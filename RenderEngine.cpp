module;

#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#define VULKAN_HPP_NO_EXCEPTIONS
#include <vulkan/vulkan.hpp>

module RenderEngine;

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

using namespace std::literals;

RenderEngine::RenderEngine(): surface(instance.get())
{
	if(window.getHasError())
	{
		hasError = true;
		return;
	}

	VULKAN_HPP_DEFAULT_DISPATCHER.init();

	std::vector<vk::ExtensionProperties> availableExtensionProperties;
	if(checkVulkanErrorOccured(availableExtensionProperties, vk::enumerateInstanceExtensionProperties(), "", "Failed to enumerate available instance extensions"))
		return;
	Logger::logInfo(std::format("{} instance extensions available:", availableExtensionProperties.size()));
	for(auto const& property : availableExtensionProperties)
		Logger::logInfo(std::format("\t{}", property.extensionName.data()));

	std::vector<vk::LayerProperties> availableLayerProperties;
	if(checkVulkanErrorOccured(availableLayerProperties, vk::enumerateInstanceLayerProperties(), "", "Failed to enumerate available validation layers"))
		return;
	Logger::logInfo(std::format("{} validation layers available:", availableLayerProperties.size()));
	for(auto const& property : availableLayerProperties)
		Logger::logInfo(std::format("\t{}", property.layerName.data()));

	auto applicationVersion = VK_MAKE_VERSION(Configuration::vkAppMajorVersion, Configuration::vkAppMinorVersion, Configuration::vkAppPatchVersion);
	auto applicationInfo = vk::ApplicationInfo(Configuration::appName.data(), applicationVersion, Configuration::appName.data(), applicationVersion, VK_API_VERSION_1_4);
	std::vector<char const*> requiredLayers;
	if constexpr(isDebugBuild)
		requiredLayers.emplace_back("VK_LAYER_KHRONOS_validation");
	Logger::logInfo(std::format("{} Vulkan validation layers required:", requiredLayers.size()));
	for(auto layer : requiredLayers)
		Logger::logInfo(std::format("\t{}", layer));

	std::unordered_set<std::string> layerSet;
	for(auto layer : requiredLayers)
		layerSet.emplace(layer);
	for(auto property : availableLayerProperties)
		layerSet.erase(property.layerName);
	if(!layerSet.empty())
	{
		hasError = true;
		Logger::logError(std::format("Required layer {} not supported", *layerSet.begin()));
		return;
	}

	auto requiredInstanceExtensions = window.getRequiredExtensions();
	if constexpr(isDebugBuild)
		requiredInstanceExtensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	Logger::logInfo(std::format("{} Vulkan instance extensions required:", requiredInstanceExtensions.size()));
	for(auto extension : requiredInstanceExtensions)
		Logger::logInfo(std::format("\t{}", extension));

	for(auto extension : requiredInstanceExtensions)
	{
		if(std::find_if(availableExtensionProperties.begin(), availableExtensionProperties.end(), [extension](auto const& property) { return std::strcmp(property.extensionName, extension) == 0; }) != availableExtensionProperties.end())
			continue;

		hasError = true;
		Logger::logError(std::format("Required extension {} not supported", extension));
		return;
	}

	vk::InstanceCreateInfo instanceCreateInfo;
	vk::DebugUtilsMessengerCreateInfoEXT messengerCreateInfo{{}, vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
			vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance | vk::DebugUtilsMessageTypeFlagBitsEXT::eDeviceAddressBinding, debugCallback};
	if constexpr(isDebugBuild)
		instanceCreateInfo = vk::InstanceCreateInfo({}, &applicationInfo, requiredLayers, requiredInstanceExtensions, &messengerCreateInfo);
	else
		instanceCreateInfo = vk::InstanceCreateInfo({}, &applicationInfo, requiredLayers, requiredInstanceExtensions);
	if(checkVulkanErrorOccured(instance, vk::createInstanceUnique(instanceCreateInfo), "Created Vulkan instance", "Failed to create Vulkan instance"))
		return;

	VULKAN_HPP_DEFAULT_DISPATCHER.init(instance.get());

	if constexpr(isDebugBuild)
	{
		if(checkVulkanErrorOccured(debugMessenger, instance->createDebugUtilsMessengerEXTUnique(messengerCreateInfo), "Created debug messenger", "Failed to create debug messenger"))
			return;
	}

	if(!window.createSurface(instance.get()))
	{
		hasError = true;
		return;
	}
	surface.surface = window.getSurface();
	Logger::logInfo("Created surface");

	std::vector<vk::PhysicalDevice> availablePhysicalDevices;
	if(checkVulkanErrorOccured(availablePhysicalDevices, instance->enumeratePhysicalDevices(), "", "Failed to enumerate physical devices"))
		return;
	if(availablePhysicalDevices.empty())
	{
		hasError = true;
		Logger::logError("No physical device with Vulkan support found. Try updating drivers");
		return;
	}

	std::vector<char const*> requiredPhysicalDeviceExtensions{VK_KHR_SWAPCHAIN_EXTENSION_NAME};
	Logger::logInfo(std::format("{} physical device extensions required:", requiredPhysicalDeviceExtensions.size()));
	for(auto const& extension : requiredPhysicalDeviceExtensions)
		Logger::logInfo(std::format("\t{}", extension));

	Logger::logInfo(std::format("{} physical devices available:", availablePhysicalDevices.size()));
	for(auto availableDevice : availablePhysicalDevices)
		Logger::logInfo(std::format("\t{}", availableDevice.getProperties().deviceName.data()));

	int32_t maxDeviceScore{0};
	PhysicalDeviceInfo physicalDeviceInfo;
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

	std::unordered_set uniqueQueueFamilyIndices{physicalDeviceInfo.graphicsIndex, physicalDeviceInfo.presentationIndex};
	std::array<float, 1> queuePriorities{1.0f};
	std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
	queueCreateInfos.reserve(uniqueQueueFamilyIndices.size());
	for(auto index : uniqueQueueFamilyIndices)
		queueCreateInfos.emplace_back(vk::DeviceQueueCreateInfo{{}, index, queuePriorities});

	vk::DeviceCreateInfo deviceCreateInfo({}, queueCreateInfos, requiredLayers, requiredPhysicalDeviceExtensions);
	if(checkVulkanErrorOccured(device, physicalDevice.createDeviceUnique(deviceCreateInfo), "Created logical device", "Failed to create logical device"))
		return;
	Logger::logInfo("Created logical device");
	VULKAN_HPP_DEFAULT_DISPATCHER.init(device.get());

	graphicsQueue = device->getQueue(physicalDeviceInfo.graphicsIndex, 0);
	presentationQueue = device->getQueue(physicalDeviceInfo.presentationIndex, 0);

	vk::SurfaceFormatKHR selectedFormat{physicalDeviceInfo.surfaceFormats[0]};
	for(auto format : physicalDeviceInfo.surfaceFormats)
	{
		if(format.format == vk::Format::eB8G8R8A8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
			selectedFormat = format;
	}
	swapchainImageFormat = selectedFormat.format;
	Logger::logInfo(std::format("Chose format {} with color space {}", vk::to_string(selectedFormat.format), vk::to_string(selectedFormat.colorSpace)));

	vk::PresentModeKHR selectedPresentMode{vk::PresentModeKHR::eFifo};
	for(auto presentMode : physicalDeviceInfo.presentModes)
	{
		if(presentMode == vk::PresentModeKHR::eMailbox)
			selectedPresentMode = presentMode;
	}
	Logger::logInfo(std::format("Chose present mode {}", vk::to_string(selectedPresentMode)));

	vk::SurfaceCapabilitiesKHR physicalDeviceSurfaceCapabilities;
	if(checkVulkanErrorOccured(physicalDeviceSurfaceCapabilities, physicalDevice.getSurfaceCapabilitiesKHR(surface), "", "Failed to get surface capabilities"))
		return;

	swapchainImageExtent = physicalDeviceSurfaceCapabilities.currentExtent;
	if(swapchainImageExtent.width == std::numeric_limits<uint32_t>::max())
	{
		auto framebufferSize = window.getFramebufferSize();
		swapchainImageExtent.width = std::clamp(framebufferSize.first, physicalDeviceSurfaceCapabilities.minImageExtent.width, physicalDeviceSurfaceCapabilities.maxImageExtent.width);
		swapchainImageExtent.height = std::clamp(framebufferSize.second, physicalDeviceSurfaceCapabilities.minImageExtent.height, physicalDeviceSurfaceCapabilities.maxImageExtent.height);
	}
	Logger::logInfo(std::format("Swap extent is [{},{}]", swapchainImageExtent.width, swapchainImageExtent.height));

	uint32_t imageCount{physicalDeviceSurfaceCapabilities.minImageCount + 1};
	if(physicalDeviceSurfaceCapabilities.maxImageCount > 0 && imageCount > physicalDeviceSurfaceCapabilities.maxImageCount)
		imageCount = physicalDeviceSurfaceCapabilities.maxImageCount;
	Logger::logInfo(std::format("Image count is {}", imageCount));

	vk::SharingMode sharingMode = physicalDeviceInfo.graphicsIndex != physicalDeviceInfo.presentationIndex ? vk::SharingMode::eConcurrent : vk::SharingMode::eExclusive;
	std::vector<uint32_t> queueFamilyIndices{sharingMode == vk::SharingMode::eConcurrent ? std::vector{physicalDeviceInfo.graphicsIndex, physicalDeviceInfo.presentationIndex} : std::vector<uint32_t>{}};
	vk::SwapchainCreateInfoKHR swapchainCreateInfo({}, surface.surface, imageCount, selectedFormat.format, selectedFormat.colorSpace, swapchainImageExtent, 1, vk::ImageUsageFlagBits::eColorAttachment, sharingMode, queueFamilyIndices, physicalDeviceSurfaceCapabilities.currentTransform, vk::CompositeAlphaFlagBitsKHR::eOpaque, selectedPresentMode, VK_TRUE);
	if(checkVulkanErrorOccured(swapchain, device->createSwapchainKHRUnique(swapchainCreateInfo), "Created swapchain", "Failed to create swapchain"))
		return;

	if(checkVulkanErrorOccured(swapchainImages, device->getSwapchainImagesKHR(swapchain.get()), "", "Failed to get swapchain images"))
		return;

	swapchainImageViews.resize(swapchainImages.size());
	for(size_t i = 0; i < swapchainImageViews.size(); i++)
	{
		vk::ImageSubresourceRange subresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
		vk::ImageViewCreateInfo viewCreateInfo({}, swapchainImages[i], vk::ImageViewType::e2D, swapchainImageFormat,
											   {vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity},
											   subresourceRange);
		if(checkVulkanErrorOccured(swapchainImageViews[i], device->createImageViewUnique(viewCreateInfo), "", "Failed to create image view"))
			return;
	}

	auto vertexShaderModule = createShaderModule("shaders/quadVert.spv");
	if(!vertexShaderModule)
		return;

	auto fragmentShaderModule = createShaderModule("shaders/quadFrag.spv");
	if(!fragmentShaderModule)
		return;

	std::vector<vk::PipelineShaderStageCreateInfo> stageCreateInfos{{{}, vk::ShaderStageFlagBits::eVertex, vertexShaderModule.get(), "main"},
																	{{}, vk::ShaderStageFlagBits::eFragment, fragmentShaderModule.get(), "main"}};

	std::vector<vk::DynamicState> dynamicStates{vk::DynamicState::eViewport, vk::DynamicState::eScissor};
	vk::PipelineDynamicStateCreateInfo dynamicStateCreateInfo({}, dynamicStates);

	vk::PipelineVertexInputStateCreateInfo vertexInputStateCreateInfo({});

	vk::PipelineInputAssemblyStateCreateInfo assemblyStateCreateInfo({}, vk::PrimitiveTopology::eTriangleStrip, VK_FALSE);

	vk::Viewport viewport(0.0f, 0.0f, swapchainImageExtent.width, swapchainImageExtent.height, 0.0f, 1.0f);
	vk::Rect2D scissor({0, 0}, swapchainImageExtent);

	vk::PipelineViewportStateCreateInfo viewportStateCreateInfo({}, viewport, scissor);

	vk::PipelineRasterizationStateCreateInfo rasterizationStateCreateInfo({}, VK_FALSE, VK_FALSE, vk::PolygonMode::eFill, vk::CullModeFlagBits::eNone, vk::FrontFace::eClockwise, VK_FALSE, 0.0f, 0.0f, 0.0f, 1.0f);

	vk::PipelineMultisampleStateCreateInfo multisampleStateCreateInfo;
	vk::PipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo;
	vk::PipelineColorBlendAttachmentState colorBlendAttachmentState;
	vk::PipelineColorBlendStateCreateInfo colorBlendStateCreateInfo({}, VK_FALSE, vk::LogicOp::eCopy, colorBlendAttachmentState);

	vk::PipelineLayoutCreateInfo layoutCreateInfo;
	if(checkVulkanErrorOccured(pipelineLayout, device->createPipelineLayoutUnique(layoutCreateInfo), "Created pipeline layout", "Failed to create pipeline layout"))
		return;

	vk::AttachmentDescription colorAttachment({}, swapchainImageFormat, vk::SampleCountFlagBits::e1,
											  vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore,
											  vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare,
											  vk::ImageLayout::eUndefined, vk::ImageLayout::ePresentSrcKHR);
	vk::AttachmentReference colorAttachmentReference(0, vk::ImageLayout::eColorAttachmentOptimal);

	vk::SubpassDescription subpassDescription({}, vk::PipelineBindPoint::eGraphics, {}, colorAttachmentReference);
	vk::SubpassDependency subpassDependency(VK_SUBPASS_EXTERNAL, 0, vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::PipelineStageFlagBits::eColorAttachmentOutput, {}, vk::AccessFlagBits::eColorAttachmentWrite);
	vk::RenderPassCreateInfo renderPassCreateInfo({}, colorAttachment, subpassDescription, subpassDependency);

	if(checkVulkanErrorOccured(renderPass, device->createRenderPassUnique(renderPassCreateInfo), "Created render pass", "Failed to create render pass"))
		return;

	vk::GraphicsPipelineCreateInfo pipelineCreateInfo({}, stageCreateInfos, &vertexInputStateCreateInfo, &assemblyStateCreateInfo,
													  nullptr, &viewportStateCreateInfo, &rasterizationStateCreateInfo,
													  &multisampleStateCreateInfo, &depthStencilStateCreateInfo, &colorBlendStateCreateInfo,
													  &dynamicStateCreateInfo, pipelineLayout.get(), renderPass.get(), 0);
	if(checkVulkanErrorOccured(graphicsPipeline, device->createGraphicsPipelineUnique({}, pipelineCreateInfo), "Created graphics pipeline", "Failed to create graphics pipeline"))
		return;

	swapchainFramebuffers.resize(swapchainImageViews.size());
	for(size_t i = 0; i < swapchainFramebuffers.size(); i++)
	{
		vk::FramebufferCreateInfo framebufferCreateInfo({}, renderPass.get(), swapchainImageViews[i].get(), swapchainImageExtent.width, swapchainImageExtent.height, 1);
		if(checkVulkanErrorOccured(swapchainFramebuffers[i], device->createFramebufferUnique(framebufferCreateInfo), "", "Failed to create swapchain buffer"))
			return;
	}
	Logger::logInfo("Created swapchain framebuffers");

	vk::CommandPoolCreateInfo poolCreateInfo(vk::CommandPoolCreateFlagBits::eResetCommandBuffer, physicalDeviceInfo.graphicsIndex);
	if(checkVulkanErrorOccured(commandPool, device->createCommandPoolUnique(poolCreateInfo), "Created command pool", "Failed to create command pool"))
		return;

	vk::CommandBufferAllocateInfo bufferAllocateInfo(commandPool.get(), vk::CommandBufferLevel::ePrimary, 1);
	if(checkVulkanErrorOccured(commandBuffers, device->allocateCommandBuffers(bufferAllocateInfo), "Allocated command buffer", "Failed to allocate command buffer"))
		return;

	vk::SemaphoreCreateInfo semaphoreCreateInfo;
	vk::FenceCreateInfo fenceCreateInfo(vk::FenceCreateFlagBits::eSignaled);
	if(checkVulkanErrorOccured(imageAvailableSemaphore, device->createSemaphoreUnique(semaphoreCreateInfo), "", "Failed to create semaphore") ||
	   checkVulkanErrorOccured(renderFinishedSemaphore, device->createSemaphoreUnique(semaphoreCreateInfo), "", "Failed to create semaphore") ||
	   checkVulkanErrorOccured(inFlightFence, device->createFenceUnique(fenceCreateInfo), "", "Failed to create fence"))
		return;
	Logger::logInfo("Created synchronization objects");
}

RenderEngine::~RenderEngine()
{
	if(device) 
		device->waitIdle();
}

bool RenderEngine::drawFrame()
{
	auto timeout = std::numeric_limits<uint64_t>::max();
	if(checkVulkanErrorOccured(device->waitForFences(inFlightFence.get(), VK_TRUE, timeout), "", "Failed to wait for fence"))
		return false;

	if(checkVulkanErrorOccured(device->resetFences(inFlightFence.get()), "", "Failed to reset fence"))
		return false;

	uint32_t imageIndex;
	if(checkVulkanErrorOccured(imageIndex, device->acquireNextImageKHR(swapchain.get(), timeout, imageAvailableSemaphore.get(), {}), "", "Failed to acquire next image"))
		return false;

	if(checkVulkanErrorOccured(commandBuffers[0].reset(), "", "Failed to reset command buffer"))
		return false;

	if(!recordCommandBuffer(commandBuffers[0], imageIndex))
		return false;

	vk::PipelineStageFlags waitStage(vk::PipelineStageFlagBits::eColorAttachmentOutput);
	vk::SubmitInfo submitInfo(imageAvailableSemaphore.get(), waitStage, commandBuffers[0], renderFinishedSemaphore.get());
	if(checkVulkanErrorOccured(graphicsQueue.submit(submitInfo, inFlightFence.get()), "", "Failed to submit to graphics queue"))
		return false;

	vk::PresentInfoKHR presentInfo(renderFinishedSemaphore.get(), swapchain.get(), imageIndex);
	if(checkVulkanErrorOccured(presentationQueue.presentKHR(presentInfo), "", "Failed to present to presentation queue"))
		return false;

	return true;
}

bool RenderEngine::recordCommandBuffer(vk::CommandBuffer commandBuffer, uint32_t imageIndex) const
{
	vk::CommandBufferBeginInfo beginInfo;
	if(checkVulkanErrorOccured(commandBuffer.begin(beginInfo), "", "Failed to begin command buffer"))
		return false;

	vk::Rect2D renderArea({0, 0}, swapchainImageExtent);
	vk::ClearValue clearValue(vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f));
	vk::RenderPassBeginInfo renderPassBeginInfo(renderPass.get(), swapchainFramebuffers[imageIndex].get(), renderArea, clearValue);
	commandBuffer.beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);

	commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, graphicsPipeline.get());

	vk::Viewport viewport(0.0f, 0.0f, swapchainImageExtent.width, swapchainImageExtent.height, 0.0f, 1.0f);
	commandBuffer.setViewport(0, viewport);

	vk::Rect2D scissor({0, 0}, swapchainImageExtent);
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
	auto& [name, formats, presentModes, graphicsIndex, presentationIndex] = result.second;

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
		if(checkVulkanErrorOccured(surfaceSupport, device.getSurfaceSupportKHR(i, surface), "", "Failed to get surface support info"))
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

	if(checkVulkanErrorOccured(formats, device.getSurfaceFormatsKHR(surface), "", "\tFailed to get surface formats"))
		return result;

	if(checkVulkanErrorOccured(presentModes, device.getSurfacePresentModesKHR(surface), "", "\tFailed to get surface present modes"))
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