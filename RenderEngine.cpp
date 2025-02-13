module;

#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#define VULKAN_HPP_NO_EXCEPTIONS
#define VULKAN_HPP_ASSERT_ON_RESULT
#include <vulkan/vulkan.hpp>

module RenderEngine;

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

using namespace std::literals;

RenderEngine::SwapchainResources::SwapchainResources(RenderEngine const& engine, RenderEngine::PhysicalDeviceInfo const& info)
{
	//Choose surface format
	vk::SurfaceFormatKHR selectedFormat{info.surfaceFormats[0]};
	for(auto format : info.surfaceFormats)
	{
		if(format.format == vk::Format::eB8G8R8A8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
			selectedFormat = format;
	}
	imageFormat = selectedFormat.format;
	Logger::logInfo(std::format("Chose format {} with color space {}",
								vk::to_string(selectedFormat.format), vk::to_string(selectedFormat.colorSpace)));

	//Choose present mode
	vk::PresentModeKHR selectedPresentMode{vk::PresentModeKHR::eFifo};
	for(auto presentMode : info.presentModes)
	{
		if(presentMode == vk::PresentModeKHR::eMailbox)
			selectedPresentMode = presentMode;
	}
	Logger::logInfo(std::format("Chose present mode {}", vk::to_string(selectedPresentMode)));

	//Choose swapchain extent
	auto const& surfaceCapabilities = info.surfaceCapabilities;
	imageExtent = surfaceCapabilities.currentExtent;
	if(imageExtent.width == std::numeric_limits<uint32_t>::max())
	{
		auto framebufferSize = engine.window.getFramebufferSize();
		imageExtent.width = std::clamp(framebufferSize.first, surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width);
		imageExtent.height = std::clamp(framebufferSize.second, surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height);
	}
	Logger::logInfo(std::format("Swap extent is [{},{}]", imageExtent.width, imageExtent.height));

	//Choose swapchain image count
	uint32_t imageCount{surfaceCapabilities.minImageCount + 1};
	if(surfaceCapabilities.maxImageCount > 0 && imageCount > surfaceCapabilities.maxImageCount)
		imageCount = surfaceCapabilities.maxImageCount;
	Logger::logInfo(std::format("Image count is {}", imageCount));

	//Create swapchain
	vk::SharingMode sharingMode{info.graphicsIndex != info.presentationIndex ? vk::SharingMode::eConcurrent : vk::SharingMode::eExclusive};
	std::vector<uint32_t> queueFamilyIndices{sharingMode == vk::SharingMode::eConcurrent ? std::vector{info.graphicsIndex, info.presentationIndex} : std::vector<uint32_t>{}};
	vk::SwapchainCreateInfoKHR swapchainCreateInfo{{}, engine.surface.get(), imageCount, selectedFormat.format, selectedFormat.colorSpace,
		imageExtent, 1, vk::ImageUsageFlagBits::eColorAttachment, sharingMode, queueFamilyIndices,
		surfaceCapabilities.currentTransform, vk::CompositeAlphaFlagBitsKHR::eOpaque, selectedPresentMode, VK_TRUE};
	if(engine.checkVulkanErrorOccured(swapchain, engine.device->createSwapchainKHRUnique(swapchainCreateInfo), "Created swapchain", "Failed to create swapchain"))
		return;

	//Get swapchain images
	if(engine.checkVulkanErrorOccured(images, engine.device->getSwapchainImagesKHR(swapchain.get()), "", "Failed to get swapchain images"))
		return;

	//Create swapchain image views
	imageViews.resize(images.size());
	for(size_t i = 0; i < imageViews.size(); i++)
	{
		vk::ImageSubresourceRange subresourceRange{vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1};
		vk::ImageViewCreateInfo viewCreateInfo{{}, images[i], vk::ImageViewType::e2D, imageFormat,
											   {vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity},
											   subresourceRange};
		if(engine.checkVulkanErrorOccured(imageViews[i], engine.device->createImageViewUnique(viewCreateInfo), "", "Failed to create image view"))
			return;
	}
}

RenderEngine::SwapchainFramebuffers::SwapchainFramebuffers(RenderEngine const& engine)
{
	//Create swapchain framebuffers
	framebuffers.resize(engine.swapchainResources.imageViews.size());
	for(size_t i = 0; i < framebuffers.size(); i++)
	{
		vk::FramebufferCreateInfo framebufferCreateInfo{{}, engine.renderPass.get(), engine.swapchainResources.imageViews[i].get(),
			engine.swapchainResources.imageExtent.width, engine.swapchainResources.imageExtent.height, 1};
		if(engine.checkVulkanErrorOccured(framebuffers[i], engine.device->createFramebufferUnique(framebufferCreateInfo), "", "Failed to create swapchain buffer"))
			return;
	}
	Logger::logInfo("Created swapchain framebuffers");
}

RenderEngine::RenderEngine()
{
	//Check if failed to initialize window
	if(window.getHasError())
	{
		hasError = true;
		return;
	}

	VULKAN_HPP_DEFAULT_DISPATCHER.init();

	//Get available instance extensions
	std::vector<vk::ExtensionProperties> availableExtensionProperties;
	if(checkVulkanErrorOccured(availableExtensionProperties, vk::enumerateInstanceExtensionProperties(),
							   "", "Failed to enumerate available instance extensions"))
		return;
	Logger::logInfo(std::format("{} instance extensions available:", availableExtensionProperties.size()));
	for(auto const& property : availableExtensionProperties)
		Logger::logInfo(std::format("\t{}", property.extensionName.data()));

	//Get available instance layers
	std::vector<vk::LayerProperties> availableLayerProperties;
	if(checkVulkanErrorOccured(availableLayerProperties, vk::enumerateInstanceLayerProperties(),
							   "", "Failed to enumerate available validation layers"))
		return;
	Logger::logInfo(std::format("{} validation layers available:", availableLayerProperties.size()));
	for(auto const& property : availableLayerProperties)
		Logger::logInfo(std::format("\t{}", property.layerName.data()));

	//Define application info
	auto applicationVersion{VK_MAKE_VERSION(Configuration::vkAppMajorVersion, Configuration::vkAppMinorVersion, Configuration::vkAppPatchVersion)};
	vk::ApplicationInfo applicationInfo{Configuration::appName.data(), applicationVersion, Configuration::appName.data(),
		applicationVersion, VK_API_VERSION_1_4};

	//Define required instance layers
	std::vector<char const*> requiredLayers;
	if constexpr(isDebugBuild)
		requiredLayers.emplace_back("VK_LAYER_KHRONOS_validation");
	Logger::logInfo(std::format("{} Vulkan validation layers required:", requiredLayers.size()));
	for(auto layer : requiredLayers)
		Logger::logInfo(std::format("\t{}", layer));

	//Check support for required instance layers
	std::unordered_set<std::string> layerSet;
	for(auto layer : requiredLayers)
		layerSet.emplace(layer);
	for(auto const& property : availableLayerProperties)
		layerSet.erase(property.layerName);
	if(!layerSet.empty())
	{
		hasError = true;
		Logger::logError(std::format("Required layer {} not supported", *layerSet.begin()));
		return;
	}

	//Define required instance extensions
	auto requiredInstanceExtensions{window.getRequiredExtensions()};
	if constexpr(isDebugBuild)
		requiredInstanceExtensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	Logger::logInfo(std::format("{} Vulkan instance extensions required:", requiredInstanceExtensions.size()));
	for(auto extension : requiredInstanceExtensions)
		Logger::logInfo(std::format("\t{}", extension));

	//Check support for required instance extensions
	std::unordered_set<std::string> extensionSet;
	for(auto extension : requiredInstanceExtensions)
		extensionSet.emplace(extension);
	for(auto const& extension : availableExtensionProperties)
		extensionSet.erase(extension.extensionName);
	if(!extensionSet.empty())
	{
		hasError = true;
		Logger::logError(std::format("Required extension {} not supported", *extensionSet.begin()));
		return;
	}

	//Create Vulkan instance
	vk::DebugUtilsMessengerCreateInfoEXT messengerCreateInfo{{},
		vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
		vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
		vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance | vk::DebugUtilsMessageTypeFlagBitsEXT::eDeviceAddressBinding,
		debugCallback};
	vk::InstanceCreateInfo instanceCreateInfo;
	if constexpr(isDebugBuild)
		instanceCreateInfo = vk::InstanceCreateInfo({}, &applicationInfo, requiredLayers, requiredInstanceExtensions, &messengerCreateInfo);
	else
		instanceCreateInfo = vk::InstanceCreateInfo({}, &applicationInfo, requiredLayers, requiredInstanceExtensions);
	if(checkVulkanErrorOccured(instance, vk::createInstanceUnique(instanceCreateInfo),
							   "Created Vulkan instance", "Failed to create Vulkan instance"))
		return;

	VULKAN_HPP_DEFAULT_DISPATCHER.init(instance.get());

	//Create debug messenger
	if constexpr(isDebugBuild)
	{
		if(checkVulkanErrorOccured(debugMessenger, instance->createDebugUtilsMessengerEXTUnique(messengerCreateInfo),
								   "Created debug messenger", "Failed to create debug messenger"))
			return;
	}

	//Create window surface
	surface = vk::UniqueSurfaceKHR(window.createSurface(instance.get()), instance.get());
	if(!surface)
	{
		hasError = true;
		return;
	}
	Logger::logInfo("Created surface");

	//Get available physical devices
	std::vector<vk::PhysicalDevice> availablePhysicalDevices;
	if(checkVulkanErrorOccured(availablePhysicalDevices, instance->enumeratePhysicalDevices(), "", "Failed to enumerate physical devices"))
		return;
	if(availablePhysicalDevices.empty())
	{
		hasError = true;
		Logger::logError("No physical device with Vulkan support found. Try updating drivers");
		return;
	}
	Logger::logInfo(std::format("{} physical devices available:", availablePhysicalDevices.size()));
	for(auto availableDevice : availablePhysicalDevices)
		Logger::logInfo(std::format("\t{}", availableDevice.getProperties().deviceName.data()));

	//Define physical device extensions
	std::vector<char const*> requiredPhysicalDeviceExtensions{VK_KHR_SWAPCHAIN_EXTENSION_NAME};
	Logger::logInfo(std::format("{} physical device extensions required:", requiredPhysicalDeviceExtensions.size()));
	for(auto const& extension : requiredPhysicalDeviceExtensions)
		Logger::logInfo(std::format("\t{}", extension));

	//Choose best physical device
	int32_t maxDeviceScore{0};
	for(auto availableDevice : availablePhysicalDevices)
	{
		auto [deviceScore, info] = getPhysicalDeviceInfo(availableDevice, requiredPhysicalDeviceExtensions);

		if(deviceScore > maxDeviceScore)
		{
			maxDeviceScore = deviceScore;
			physicalDevice = availableDevice;
			physicalDeviceInfo = info;
		}
	}
	if(!physicalDevice)
	{
		hasError = true;
		Logger::logError("No suitable physical devices found. Try updating drivers");
		return;
	}
	Logger::logInfo(std::format("Picked {} as a suitable physical device", physicalDeviceInfo.name));

	//Define device queues
	std::unordered_set uniqueQueueFamilyIndices{physicalDeviceInfo.graphicsIndex, physicalDeviceInfo.presentationIndex};
	std::array<float, 1> queuePriorities{1.0f};
	std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
	queueCreateInfos.reserve(uniqueQueueFamilyIndices.size());
	for(auto index : uniqueQueueFamilyIndices)
		queueCreateInfos.emplace_back(vk::DeviceQueueCreateFlags{}, index, queuePriorities);

	//Create logical device
	vk::DeviceCreateInfo deviceCreateInfo{{}, queueCreateInfos, requiredLayers, requiredPhysicalDeviceExtensions};
	if(checkVulkanErrorOccured(device, physicalDevice.createDeviceUnique(deviceCreateInfo),
							   "Created logical device", "Failed to create logical device"))
		return;
	Logger::logInfo("Created logical device");

	VULKAN_HPP_DEFAULT_DISPATCHER.init(device.get());

	//Get queues
	graphicsQueue = device->getQueue(physicalDeviceInfo.graphicsIndex, 0);
	presentationQueue = device->getQueue(physicalDeviceInfo.presentationIndex, 0);

	swapchainResources = SwapchainResources(*this, physicalDeviceInfo);

	//Create shader modules
	auto vertexShaderModule = createShaderModule("shaders/quadVert.spv");
	if(!vertexShaderModule)
		return;
	auto fragmentShaderModule = createShaderModule("shaders/quadFrag.spv");
	if(!fragmentShaderModule)
		return;

	//Define shader stages
	std::vector<vk::PipelineShaderStageCreateInfo> stageCreateInfos{{{}, vk::ShaderStageFlagBits::eVertex, vertexShaderModule.get(), "main"},
																	{{}, vk::ShaderStageFlagBits::eFragment, fragmentShaderModule.get(), "main"}};

	//Define dynamic states
	std::vector<vk::DynamicState> dynamicStates{vk::DynamicState::eViewport, vk::DynamicState::eScissor};
	vk::PipelineDynamicStateCreateInfo dynamicStateCreateInfo{{}, dynamicStates};

	//Define vertex input
	vk::PipelineVertexInputStateCreateInfo vertexInputStateCreateInfo{{}, nullptr, nullptr};

	//Define input assembly
	vk::PipelineInputAssemblyStateCreateInfo assemblyStateCreateInfo{{}, vk::PrimitiveTopology::eTriangleStrip, VK_FALSE};

	//Define viewport
	vk::Viewport viewport{0.0f, 0.0f, (float)swapchainResources.imageExtent.width, (float)swapchainResources.imageExtent.height, 0.0f, 1.0f};
	vk::Rect2D scissor{{0, 0}, swapchainResources.imageExtent};
	vk::PipelineViewportStateCreateInfo viewportStateCreateInfo{{}, viewport, scissor};

	//Define rasterization
	vk::PipelineRasterizationStateCreateInfo rasterizationStateCreateInfo{{}, VK_FALSE, VK_FALSE, vk::PolygonMode::eFill,
																		  vk::CullModeFlagBits::eBack, vk::FrontFace::eClockwise, VK_FALSE,
																		  0.0f, 0.0f, 0.0f, 1.0f};

	//Define multisampling
	vk::PipelineMultisampleStateCreateInfo multisampleStateCreateInfo{{}, vk::SampleCountFlagBits::e1, VK_FALSE, 0.0, nullptr, VK_FALSE, VK_FALSE};

	//Define depth and stencil
	vk::PipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo{{}, VK_FALSE, VK_FALSE, vk::CompareOp::eNever, VK_FALSE, VK_FALSE};

	//Define color blending
	vk::PipelineColorBlendAttachmentState colorBlendAttachmentState{VK_FALSE, vk::BlendFactor::eSrcAlpha, vk::BlendFactor::eOneMinusSrcAlpha,
																	vk::BlendOp::eAdd, vk::BlendFactor::eOne, vk::BlendFactor::eZero, vk::BlendOp::eAdd,
																	vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
																	vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA};
	vk::PipelineColorBlendStateCreateInfo colorBlendStateCreateInfo{{}, VK_FALSE, vk::LogicOp::eNoOp, colorBlendAttachmentState, {1.0f, 1.0f, 1.0f, 1.0f}};

	//Create pipeline layout
	vk::PipelineLayoutCreateInfo layoutCreateInfo;
	if(checkVulkanErrorOccured(pipelineLayout, device->createPipelineLayoutUnique(layoutCreateInfo),
							   "Created pipeline layout", "Failed to create pipeline layout"))
		return;

	//Define attachment
	vk::AttachmentDescription colorAttachment{{}, swapchainResources.imageFormat, vk::SampleCountFlagBits::e1,
											  vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore,
											  vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare,
											  vk::ImageLayout::eUndefined, vk::ImageLayout::ePresentSrcKHR};
	vk::AttachmentReference colorAttachmentReference{0, vk::ImageLayout::eColorAttachmentOptimal};

	//Create render pass
	vk::SubpassDescription subpassDescription{{}, vk::PipelineBindPoint::eGraphics, {}, colorAttachmentReference};
	vk::SubpassDependency subpassDependency{VK_SUBPASS_EXTERNAL, 0, vk::PipelineStageFlagBits::eColorAttachmentOutput,
											vk::PipelineStageFlagBits::eColorAttachmentOutput, {}, vk::AccessFlagBits::eColorAttachmentWrite};
	vk::RenderPassCreateInfo renderPassCreateInfo{{}, colorAttachment, subpassDescription, subpassDependency};
	if(checkVulkanErrorOccured(renderPass, device->createRenderPassUnique(renderPassCreateInfo),
							   "Created render pass", "Failed to create render pass"))
		return;

	//Create graphics pipeline
	vk::GraphicsPipelineCreateInfo pipelineCreateInfo({}, stageCreateInfos, &vertexInputStateCreateInfo, &assemblyStateCreateInfo,
													  nullptr, &viewportStateCreateInfo, &rasterizationStateCreateInfo,
													  &multisampleStateCreateInfo, &depthStencilStateCreateInfo, &colorBlendStateCreateInfo,
													  &dynamicStateCreateInfo, pipelineLayout.get(), renderPass.get(), 0);
	if(checkVulkanErrorOccured(graphicsPipeline, device->createGraphicsPipelineUnique({}, pipelineCreateInfo),
							   "Created graphics pipeline", "Failed to create graphics pipeline"))
		return;

	swapchainFramebuffers = SwapchainFramebuffers(*this);

	//Create command pool
	vk::CommandPoolCreateInfo poolCreateInfo{vk::CommandPoolCreateFlagBits::eResetCommandBuffer, physicalDeviceInfo.graphicsIndex};
	if(checkVulkanErrorOccured(commandPool, device->createCommandPoolUnique(poolCreateInfo), "Created command pool", "Failed to create command pool"))
		return;

	//Allocate command buffers
	vk::CommandBufferAllocateInfo bufferAllocateInfo{commandPool.get(), vk::CommandBufferLevel::ePrimary, maxFramesInFlight};
	if(checkVulkanErrorOccured(commandBuffers, device->allocateCommandBuffers(bufferAllocateInfo), "Allocated command buffer", "Failed to allocate command buffer"))
		return;

	//Create synchronization objects
	vk::SemaphoreCreateInfo semaphoreCreateInfo;
	vk::FenceCreateInfo fenceCreateInfo{vk::FenceCreateFlagBits::eSignaled};
	for(uint64_t i{0}; i < maxFramesInFlight; i++)
	{
		if(checkVulkanErrorOccured(imageAvailableSemaphores[i], device->createSemaphoreUnique(semaphoreCreateInfo), "", "Failed to create semaphore") ||
		   checkVulkanErrorOccured(renderFinishedSemaphores[i], device->createSemaphoreUnique(semaphoreCreateInfo), "", "Failed to create semaphore") ||
		   checkVulkanErrorOccured(inFlightFences[i], device->createFenceUnique(fenceCreateInfo), "", "Failed to create fence"))
			return;
	}
	Logger::logInfo("Created synchronization objects");
}

RenderEngine::~RenderEngine()
{
	//Wait until rendering is finished before cleanup
	if(device)
		device->waitIdle();
}

bool RenderEngine::drawFrame()
{
	auto timeout = std::numeric_limits<uint64_t>::max();
	if(checkVulkanErrorOccured(device->waitForFences(inFlightFences[currentFrameIndex].get(), VK_TRUE, timeout), "", "Failed to wait for fence"))
		return false;

	auto [result, imageIndex] = device->acquireNextImageKHR(swapchainResources.swapchain.get(), timeout, imageAvailableSemaphores[currentFrameIndex].get(), {});
	if(result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR)
		return recreateSwapchain();
	else
		checkVulkanErrorOccured(result, "", "Failed to acquire next image");

	if(checkVulkanErrorOccured(device->resetFences(inFlightFences[currentFrameIndex].get()), "", "Failed to reset fence"))
		return false;

	if(checkVulkanErrorOccured(commandBuffers[currentFrameIndex].reset(), "", "Failed to reset command buffer"))
		return false;

	if(!recordCommandBuffer(commandBuffers[currentFrameIndex], imageIndex))
		return false;

	vk::PipelineStageFlags waitStage(vk::PipelineStageFlagBits::eColorAttachmentOutput);
	vk::SubmitInfo submitInfo(imageAvailableSemaphores[currentFrameIndex].get(), waitStage, commandBuffers[currentFrameIndex], renderFinishedSemaphores[currentFrameIndex].get());
	if(checkVulkanErrorOccured(graphicsQueue.submit(submitInfo, inFlightFences[currentFrameIndex].get()), "", "Failed to submit to graphics queue"))
		return false;

	vk::PresentInfoKHR presentInfo(renderFinishedSemaphores[currentFrameIndex].get(), swapchainResources.swapchain.get(), imageIndex);
	result = presentationQueue.presentKHR(presentInfo);
	if(result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR)
		return recreateSwapchain();
	else
		checkVulkanErrorOccured(result, "", "Failed to present image");

	currentFrameIndex = (currentFrameIndex + 1) % maxFramesInFlight;

	return true;
}

bool RenderEngine::recreateSwapchain()
{
	device->waitIdle();

	swapchainFramebuffers = {};
	swapchainResources = {};

	swapchainResources = SwapchainResources(*this, physicalDeviceInfo);
	if(hasError) 
		return false;

	swapchainFramebuffers = SwapchainFramebuffers(*this);
	if(hasError)
		return false;

	return true;
}

bool RenderEngine::recordCommandBuffer(vk::CommandBuffer commandBuffer, uint32_t imageIndex) const
{
	vk::CommandBufferBeginInfo beginInfo;
	if(checkVulkanErrorOccured(commandBuffer.begin(beginInfo), "", "Failed to begin command buffer"))
		return false;

	vk::Rect2D renderArea({0, 0}, swapchainResources.imageExtent);
	vk::ClearValue clearValue(vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f));
	vk::RenderPassBeginInfo renderPassBeginInfo(renderPass.get(), swapchainFramebuffers.framebuffers[imageIndex].get(), renderArea, clearValue);
	commandBuffer.beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);

	commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, graphicsPipeline.get());

	vk::Viewport viewport(0.0f, 0.0f, swapchainResources.imageExtent.width, swapchainResources.imageExtent.height, 0.0f, 1.0f);
	commandBuffer.setViewport(0, viewport);

	vk::Rect2D scissor({0, 0}, swapchainResources.imageExtent);
	commandBuffer.setScissor(0, scissor);

	commandBuffer.draw(4, 1, 0, 0);

	commandBuffer.endRenderPass();

	if(checkVulkanErrorOccured(commandBuffer.end(), "", "Failed to end command buffer"))
		return false;

	return true;
}

template<class Value, class Result>
bool RenderEngine::checkVulkanErrorOccured(Value& value, Result result, std::string_view successMessage, std::string_view errorMessage) const
{
	if(result.result != vk::Result::eSuccess)
	{
		hasError = true;
		auto errorString = errorMessage.data() + ": "s + vk::to_string(result.result);
		Logger::logError(errorString);
		return true;
	}
	else if(!successMessage.empty())
		Logger::logInfo(successMessage);

	value = std::move(result.value);
	return false;
}

bool RenderEngine::checkVulkanErrorOccured(vk::Result result, std::string_view successMessage, std::string_view errorMessage) const
{
	if(result != vk::Result::eSuccess)
	{
		hasError = true;
		auto errorString = errorMessage.data() + ": "s + vk::to_string(result);
		Logger::logError(errorString);
		return true;
	}
	else if(!successMessage.empty())
		Logger::logInfo(successMessage);

	return false;
}

std::pair<int32_t, RenderEngine::PhysicalDeviceInfo> RenderEngine::getPhysicalDeviceInfo(vk::PhysicalDevice device, std::vector<char const*> const& requiredExtensions) const
{
	std::pair<int32_t, PhysicalDeviceInfo> result;
	result.first = -1;
	auto& [name, formats, presentModes, surfaceCapabilities, graphicsIndex, presentationIndex] = result.second;

	auto deviceProperties = device.getProperties();
	name = deviceProperties.deviceName.data();
	Logger::logInfo(std::format("Checking {} suitability:", name));

	auto queueFamilyProperties = device.getQueueFamilyProperties();
	Logger::logInfo(std::format("\t{} queue families available", queueFamilyProperties.size()));

	bool hasGraphicsQueueFamily{};
	bool hasPresentationQueueFamily{};
	for(size_t i{}; i < queueFamilyProperties.size(); i++)
	{
		if(queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eGraphics)
		{
			hasGraphicsQueueFamily = true;
			graphicsIndex = i;
			Logger::logInfo(std::format("\tQueue family with index {} supports graphics", i));
		}

		vk::Bool32 surfaceSupport;
		if(checkVulkanErrorOccured(surfaceSupport, device.getSurfaceSupportKHR(i, surface.get()), "", "Failed to get surface support info"))
			return result;
		if(surfaceSupport)
		{
			hasPresentationQueueFamily = true;
			presentationIndex = i;
			Logger::logInfo(std::format("\tQueue family with index {} supports presentation", i));
		}

		if(hasGraphicsQueueFamily && hasPresentationQueueFamily)
			break;
	}
	if(!hasGraphicsQueueFamily)
	{
		Logger::logInfo("\tNo queue family with graphics support found");
		return result;
	}
	if(!hasPresentationQueueFamily)
	{
		Logger::logInfo("\tNo queue family with presentation support found");
		return result;
	}

	std::vector<vk::ExtensionProperties> deviceExtensions;
	if(checkVulkanErrorOccured(deviceExtensions, device.enumerateDeviceExtensionProperties(), "", "Failed to enumerate physical device extension properties"))
		return result;
	Logger::logInfo(std::format("\t{} physical device extensions available:", deviceExtensions.size()));
	std::unordered_set<std::string> extensionSet;
	for(auto extension : requiredExtensions)
		extensionSet.insert(extension);
	for(auto const& extension : deviceExtensions)
	{
		extensionSet.erase(extension.extensionName);
		Logger::logInfo(std::format("\t\t{}", extension.extensionName.data()));
	}

	if(!extensionSet.empty())
	{
		Logger::logInfo(std::format("\tRequired physical device extension {} not supported", *extensionSet.begin()));
		return result;
	}

	if(checkVulkanErrorOccured(formats, device.getSurfaceFormatsKHR(surface.get()), "", "\tFailed to get surface formats"))
		return result;

	if(checkVulkanErrorOccured(presentModes, device.getSurfacePresentModesKHR(surface.get()), "", "\tFailed to get surface present modes"))
		return result;

	if(checkVulkanErrorOccured(surfaceCapabilities, device.getSurfaceCapabilitiesKHR(surface.get()), "", "\tFailed to get surface capabilities"))
		return result;

	if(formats.empty() || presentModes.empty())
	{
		Logger::logInfo("\tPhysical device doesn't support swapchain");
		return result;
	}

	result.first = 0;
	if(deviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
		result.first++;
	Logger::logInfo(std::format("\tPhysical device is a {}", vk::to_string(deviceProperties.deviceType)));

	return result;
}

vk::UniqueShaderModule RenderEngine::createShaderModule(std::string_view shaderFileName) const
{
	vk::UniqueShaderModule result;
	std::ifstream shaderFile(shaderFileName.data(), std::ios::ate | std::ios::binary | std::ios::in);

	if(!shaderFile)
	{
		hasError = true;
		Logger::logError(std::format("Failed to open shader file {}", shaderFileName.data()));
		return result;
	}

	size_t fileSize{static_cast<size_t>(std::streamoff(shaderFile.tellg()))};
	std::vector<uint32_t> fileBuffer(fileSize / 4 + 1);
	shaderFile.seekg(0);
	shaderFile.read(reinterpret_cast<char*>(fileBuffer.data()), fileSize);

	vk::ShaderModuleCreateInfo createInfo({}, fileSize, fileBuffer.data());
	if(checkVulkanErrorOccured(result, device->createShaderModuleUnique(createInfo), "Created shader module "s + shaderFileName.data(), "Failed to create shader module "s + shaderFileName.data()))
		return result;

	return result;
}

VKAPI_ATTR VkBool32 VKAPI_CALL RenderEngine::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
														   VkDebugUtilsMessageTypeFlagsEXT messageType,
														   const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
														   void* pUserData)
{
	if(messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
		Logger::logInfo(std::format("{}", pCallbackData->pMessage));
	else if(messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
		Logger::logError(std::format("{}", pCallbackData->pMessage));

	return VK_FALSE;
}