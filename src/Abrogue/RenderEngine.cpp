module;

#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#define VULKAN_HPP_NO_EXCEPTIONS
#define VULKAN_HPP_ASSERT_ON_RESULT
#include <vulkan/vulkan.hpp>

module RenderEngine;

import ImageLoader;
import GameSystems;

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

using namespace std::literals;

bool RenderEngine::SwapchainResources::createSwapchain(RenderEngine const& engine)
{
	auto const& info = engine.physicalDeviceInfo;

	//Choose surface format
	vk::SurfaceFormatKHR selectedFormat{info.surfaceFormats[0]};
	for(auto format : info.surfaceFormats)
	{
		if(format.format == vk::Format::eB8G8R8A8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
			selectedFormat = format;
	}
	imageFormat = selectedFormat.format;
	logger.logInfo(std::format("Chose format {} with color space {}",
								vk::to_string(selectedFormat.format), vk::to_string(selectedFormat.colorSpace)));

	//Choose present mode
	vk::PresentModeKHR selectedPresentMode{vk::PresentModeKHR::eFifo};
	for(auto presentMode : info.presentModes)
	{
		if(presentMode == vk::PresentModeKHR::eMailbox)
			selectedPresentMode = presentMode;
	}
	logger.logInfo(std::format("Chose present mode {}", vk::to_string(selectedPresentMode)));

	//Choose swapchain extent
	auto const& surfaceCapabilities = info.surfaceCapabilities;
	imageExtent = surfaceCapabilities.currentExtent;
	if(imageExtent.width == std::numeric_limits<uint32_t>::max())
	{
		auto framebufferSize = engine.window.getFramebufferSize();
		imageExtent.width = std::clamp(framebufferSize.first, surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width);
		imageExtent.height = std::clamp(framebufferSize.second, surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height);
	}
	logger.logInfo(std::format("Swap extent is [{},{}]", imageExtent.width, imageExtent.height));

	//Choose swapchain image count
	uint32_t imageCount{surfaceCapabilities.minImageCount + 1};
	if(surfaceCapabilities.maxImageCount > 0 && imageCount > surfaceCapabilities.maxImageCount)
		imageCount = surfaceCapabilities.maxImageCount;
	logger.logInfo(std::format("Image count is {}", imageCount));

	//Create swapchain
	vk::SharingMode sharingMode{info.graphicsIndex != info.presentationIndex ? vk::SharingMode::eConcurrent : vk::SharingMode::eExclusive};
	std::vector<uint32_t> queueFamilyIndices{sharingMode == vk::SharingMode::eConcurrent ? std::vector{info.graphicsIndex, info.presentationIndex} : std::vector<uint32_t>{}};
	vk::SwapchainCreateInfoKHR swapchainCreateInfo{{}, engine.surface.get(), imageCount, selectedFormat.format, selectedFormat.colorSpace,
		imageExtent, 1, vk::ImageUsageFlagBits::eColorAttachment, sharingMode, queueFamilyIndices,
		surfaceCapabilities.currentTransform, vk::CompositeAlphaFlagBitsKHR::eOpaque, selectedPresentMode, VK_TRUE, engine.oldSwapchainResources.swapchain.get()};
	if(engine.checkVulkanErrorOccured(swapchain, engine.device->createSwapchainKHRUnique(swapchainCreateInfo), "Created swapchain", "Failed to create swapchain"))
		return false;

	//Get swapchain images
	if(engine.checkVulkanErrorOccured(images, engine.device->getSwapchainImagesKHR(swapchain.get()), "", "Failed to get swapchain images"))
		return false;

	//Create swapchain image views
	imageViews.resize(images.size());
	for(size_t i = 0; i < imageViews.size(); i++)
	{
		vk::ImageSubresourceRange subresourceRange{vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1};
		vk::ImageViewCreateInfo viewCreateInfo{{}, images[i], vk::ImageViewType::e2D, imageFormat,
											   {vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity},
											   subresourceRange};
		if(engine.checkVulkanErrorOccured(imageViews[i], engine.device->createImageViewUnique(viewCreateInfo), "", "Failed to create image view"))
			return false;
	}

	//Define attachment
	vk::AttachmentDescription colorAttachment{{}, imageFormat, vk::SampleCountFlagBits::e1,
											  vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore,
											  vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare,
											  vk::ImageLayout::eUndefined, vk::ImageLayout::ePresentSrcKHR};
	vk::AttachmentReference colorAttachmentReference{0, vk::ImageLayout::eColorAttachmentOptimal};

	//Create render pass
	vk::SubpassDescription subpassDescription{{}, vk::PipelineBindPoint::eGraphics, {}, colorAttachmentReference};
	vk::SubpassDependency subpassDependency{VK_SUBPASS_EXTERNAL, 0, vk::PipelineStageFlagBits::eColorAttachmentOutput,
											vk::PipelineStageFlagBits::eColorAttachmentOutput, {}, vk::AccessFlagBits::eColorAttachmentWrite};
	vk::RenderPassCreateInfo renderPassCreateInfo{{}, colorAttachment, subpassDescription, subpassDependency};
	if(engine.checkVulkanErrorOccured(renderPass, engine.device->createRenderPassUnique(renderPassCreateInfo),
									  "Created render pass", "Failed to create render pass"))
		return false;

	//Create swapchain framebuffers
	framebuffers.resize(imageViews.size());
	for(size_t i = 0; i < framebuffers.size(); i++)
	{
		vk::FramebufferCreateInfo framebufferCreateInfo{{}, renderPass.get(), imageViews[i].get(),
			imageExtent.width, imageExtent.height, 1};
		if(engine.checkVulkanErrorOccured(framebuffers[i], engine.device->createFramebufferUnique(framebufferCreateInfo), "", "Failed to create swapchain buffer"))
			return false;
	}
	logger.logInfo("Created swapchain framebuffers");

	return true;
}

bool RenderEngine::initVulkan()
{
	//Check if failed to initialize window
	if(!window.initSDL())
		return false;

	VULKAN_HPP_DEFAULT_DISPATCHER.init(dynamicLoader);

	//Get available instance extensions
	std::vector<vk::ExtensionProperties> availableExtensionProperties;
	if(checkVulkanErrorOccured(availableExtensionProperties, vk::enumerateInstanceExtensionProperties(),
							   "", "Failed to enumerate available instance extensions"))
		return false;
	logger.logInfo(std::format("{} instance extensions available:", availableExtensionProperties.size()));
	for(auto const& property : availableExtensionProperties)
		logger.logInfo(std::format("\t{}", property.extensionName.data()));

	//Get available instance layers
	std::vector<vk::LayerProperties> availableLayerProperties;
	if(checkVulkanErrorOccured(availableLayerProperties, vk::enumerateInstanceLayerProperties(),
							   "", "Failed to enumerate available validation layers"))
		return false;
	logger.logInfo(std::format("{} validation layers available:", availableLayerProperties.size()));
	for(auto const& property : availableLayerProperties)
		logger.logInfo(std::format("\t{}", property.layerName.data()));

	//Define application info
	auto applicationVersion{VK_MAKE_VERSION(Constants::vkAppMajorVersion, Constants::vkAppMinorVersion, Constants::vkAppPatchVersion)};
	vk::ApplicationInfo applicationInfo{Constants::appName.data(), applicationVersion, Constants::appName.data(),
		applicationVersion, VK_API_VERSION_1_4};

	//Define required instance layers
	std::vector<char const*> requiredLayers;
	if constexpr(isDebugBuild)
		requiredLayers.emplace_back("VK_LAYER_KHRONOS_validation");
	logger.logInfo(std::format("{} Vulkan validation layers required:", requiredLayers.size()));
	for(auto layer : requiredLayers)
		logger.logInfo(std::format("\t{}", layer));

	//Check support for required instance layers
	std::unordered_set<std::string> layerSet;
	for(auto layer : requiredLayers)
		layerSet.emplace(layer);
	for(auto const& property : availableLayerProperties)
		layerSet.erase(property.layerName);
	if(!layerSet.empty())
	{
		logger.logError(std::format("Required layer {} not supported", *layerSet.begin()));
		return false;
	}

	//Define required instance extensions
	auto requiredInstanceExtensions{window.getRequiredExtensions()};
	if constexpr(isDebugBuild)
		requiredInstanceExtensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	logger.logInfo(std::format("{} Vulkan instance extensions required:", requiredInstanceExtensions.size()));
	for(auto extension : requiredInstanceExtensions)
		logger.logInfo(std::format("\t{}", extension));

	//Check support for required instance extensions
	std::unordered_set<std::string> extensionSet;
	for(auto extension : requiredInstanceExtensions)
		extensionSet.emplace(extension);
	for(auto const& extension : availableExtensionProperties)
		extensionSet.erase(extension.extensionName);
	if(!extensionSet.empty())
	{
		logger.logError(std::format("Required extension {} not supported", *extensionSet.begin()));
		return false;
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
		return false;

	VULKAN_HPP_DEFAULT_DISPATCHER.init(instance.get());

	//Create debug messenger
	if constexpr(isDebugBuild)
	{
		if(checkVulkanErrorOccured(debugMessenger, instance->createDebugUtilsMessengerEXTUnique(messengerCreateInfo),
								   "Created debug messenger", "Failed to create debug messenger"))
			return false;
	}

	//Create window surface
	surface = vk::UniqueSurfaceKHR(window.createSurface(instance.get()), instance.get());
	if(!surface)
		return false;
	logger.logInfo("Created surface");

	//Get available physical devices
	std::vector<vk::PhysicalDevice> availablePhysicalDevices;
	if(checkVulkanErrorOccured(availablePhysicalDevices, instance->enumeratePhysicalDevices(), "", "Failed to enumerate physical devices"))
		return false;
	if(availablePhysicalDevices.empty())
	{
		logger.logError("No physical device with Vulkan support found. Try updating drivers");
		return false;
	}
	logger.logInfo(std::format("{} physical devices available:", availablePhysicalDevices.size()));
	for(auto availableDevice : availablePhysicalDevices)
		logger.logInfo(std::format("\t{}", availableDevice.getProperties().deviceName.data()));

	//Define physical device extensions
	std::vector<char const*> requiredPhysicalDeviceExtensions{VK_KHR_SWAPCHAIN_EXTENSION_NAME};
	logger.logInfo(std::format("{} physical device extensions required:", requiredPhysicalDeviceExtensions.size()));
	for(auto const& extension : requiredPhysicalDeviceExtensions)
		logger.logInfo(std::format("\t{}", extension));

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
		logger.logError("No suitable physical devices found. Try updating drivers");
		return false;
	}
	logger.logInfo(std::format("Picked {} as a suitable physical device", physicalDeviceInfo.name));

	//Define device queues
	std::unordered_set uniqueQueueFamilyIndices{physicalDeviceInfo.graphicsIndex, physicalDeviceInfo.presentationIndex};
	std::array<float, 1> queuePriorities{1.0f};
	std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
	queueCreateInfos.reserve(uniqueQueueFamilyIndices.size());
	for(auto index : uniqueQueueFamilyIndices)
		queueCreateInfos.emplace_back(vk::DeviceQueueCreateFlags{}, index, queuePriorities);

	//Create logical device
	vk::PhysicalDeviceVulkan12Features features12;
	features12.bufferDeviceAddress = VK_TRUE;
	features12.scalarBlockLayout = VK_TRUE;
	vk::PhysicalDeviceVulkan11Features features11;
	features11.pNext = &features12;
	vk::PhysicalDeviceFeatures2 requiredPhysicalDeviceFeatures({}, &features11);
	requiredPhysicalDeviceFeatures.features.shaderInt64 = VK_TRUE;
	requiredPhysicalDeviceFeatures.features.samplerAnisotropy = VK_TRUE;
	vk::DeviceCreateInfo deviceCreateInfo{{}, queueCreateInfos, requiredLayers, requiredPhysicalDeviceExtensions, nullptr, &requiredPhysicalDeviceFeatures};
	if(checkVulkanErrorOccured(device, physicalDevice.createDeviceUnique(deviceCreateInfo),
							   "Created logical device", "Failed to create logical device"))
		return false;
	logger.logInfo("Created logical device");

	VULKAN_HPP_DEFAULT_DISPATCHER.init(device.get());

	//Get queues
	graphicsQueue = device->getQueue(physicalDeviceInfo.graphicsIndex, 0);
	presentationQueue = device->getQueue(physicalDeviceInfo.presentationIndex, 0);

	if(!swapchainResources.createSwapchain(*this))
		return false;

	//Create command pool
	vk::CommandPoolCreateInfo poolCreateInfo{vk::CommandPoolCreateFlagBits::eResetCommandBuffer, physicalDeviceInfo.graphicsIndex};
	if(checkVulkanErrorOccured(commandPool, device->createCommandPoolUnique(poolCreateInfo), "Created command pool", "Failed to create command pool"))
		return false;

	if(!textureResources.createTexture(*this, "textures/tiles.png"))
		return false;

	vk::DescriptorSetLayoutBinding layoutBinding(0, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment, {});
	vk::DescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo({}, layoutBinding);
	if(checkVulkanErrorOccured(descriptorSetLayout, device->createDescriptorSetLayoutUnique(descriptorSetLayoutCreateInfo), "Created descriptor set layout", "Failed to create descriptor set layout"))
		return false;

	vk::DescriptorPoolSize descriptorPoolSize(vk::DescriptorType::eSampler, maxFramesInFlight);
	vk::DescriptorPoolCreateInfo descriptorPoolCreateInfo({}, maxFramesInFlight, descriptorPoolSize);
	if(checkVulkanErrorOccured(descriptorPool, device->createDescriptorPoolUnique(descriptorPoolCreateInfo), "Created descriptor pool", "Failed to create descriptor pool"))
		return false;

	std::array<vk::DescriptorSetLayout, 2> setLayouts{descriptorSetLayout.get(), descriptorSetLayout.get()};
	vk::DescriptorSetAllocateInfo descriptorSetAllocateInfo(descriptorPool.get(), setLayouts);
	std::vector<vk::DescriptorSet> allocatedSets;
	if(checkVulkanErrorOccured(allocatedSets, device->allocateDescriptorSets(descriptorSetAllocateInfo), "Allocated descriptor sets", "Failed to allocate descriptor sets"))
		return false;
	for(size_t i = 0; i < allocatedSets.size(); i++)
		descriptorSets[i] = allocatedSets[i];

	for(size_t i = 0; i < maxFramesInFlight; i++)
	{
		vk::DescriptorImageInfo imageInfo(textureResources.sampler.get(), textureResources.imageView.get(), vk::ImageLayout::eShaderReadOnlyOptimal);
		vk::WriteDescriptorSet writeDescriptorSet(descriptorSets[i], 0, 0, vk::DescriptorType::eCombinedImageSampler, imageInfo);
		device->updateDescriptorSets(writeDescriptorSet, {});
	}

	//Create shader modules
	auto vertexShaderModule = createShaderModule("shaders/quadVert.spv");
	if(!vertexShaderModule)
		return false;
	auto fragmentShaderModule = createShaderModule("shaders/quadFrag.spv");
	if(!fragmentShaderModule)
		return false;

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
	vk::PipelineColorBlendAttachmentState colorBlendAttachmentState{VK_TRUE, vk::BlendFactor::eSrcAlpha, vk::BlendFactor::eOneMinusSrcAlpha,
																	vk::BlendOp::eAdd, vk::BlendFactor::eOne, vk::BlendFactor::eZero, vk::BlendOp::eAdd,
																	vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
																	vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA};
	vk::PipelineColorBlendStateCreateInfo colorBlendStateCreateInfo{{}, VK_FALSE, vk::LogicOp::eNoOp, colorBlendAttachmentState, {1.0f, 1.0f, 1.0f, 1.0f}};

	//Create pipeline layout
	vk::PushConstantRange pushConstantRange(vk::ShaderStageFlagBits::eVertex, 0, sizeof(QuadData));
	vk::PipelineLayoutCreateInfo layoutCreateInfo({}, descriptorSetLayout.get(), pushConstantRange);
	if(checkVulkanErrorOccured(pipelineLayout, device->createPipelineLayoutUnique(layoutCreateInfo),
							   "Created pipeline layout", "Failed to create pipeline layout"))
		return false;

	//Create graphics pipeline
	vk::GraphicsPipelineCreateInfo pipelineCreateInfo({}, stageCreateInfos, &vertexInputStateCreateInfo, &assemblyStateCreateInfo,
													  nullptr, &viewportStateCreateInfo, &rasterizationStateCreateInfo,
													  &multisampleStateCreateInfo, &depthStencilStateCreateInfo, &colorBlendStateCreateInfo,
													  &dynamicStateCreateInfo, pipelineLayout.get(), swapchainResources.renderPass.get(), 0);
	if(checkVulkanErrorOccured(graphicsPipeline, device->createGraphicsPipelineUnique({}, pipelineCreateInfo),
							   "Created graphics pipeline", "Failed to create graphics pipeline"))
		return false;

	for(uint64_t i{0}; i < quadDataBuffers.size(); i++)
	{
		if(!quadDataBuffers[i].createBuffer(*this, quadPool.getCapacity(), vk::BufferUsageFlagBits::eShaderDeviceAddress))
			return false;
	}
	logger.logInfo("Created quad data buffers");

	//Allocate command buffers
	vk::CommandBufferAllocateInfo bufferAllocateInfo{commandPool.get(), vk::CommandBufferLevel::ePrimary, maxFramesInFlight};
	std::vector<vk::CommandBuffer> allocatedBuffers;
	if(checkVulkanErrorOccured(allocatedBuffers, device->allocateCommandBuffers(bufferAllocateInfo), "Allocated command buffer", "Failed to allocate command buffer"))
		return false;
	for(size_t i = 0; i < allocatedBuffers.size(); i++)
		commandBuffers[i] = allocatedBuffers[i];

	//Create synchronization objects
	vk::SemaphoreCreateInfo semaphoreCreateInfo;
	vk::FenceCreateInfo fenceCreateInfo{vk::FenceCreateFlagBits::eSignaled};
	for(uint64_t i{0}; i < maxFramesInFlight; i++)
	{
		if(checkVulkanErrorOccured(imageAvailableSemaphores[i], device->createSemaphoreUnique(semaphoreCreateInfo), "", "Failed to create semaphore") ||
		   checkVulkanErrorOccured(renderFinishedSemaphores[i], device->createSemaphoreUnique(semaphoreCreateInfo), "", "Failed to create semaphore") ||
		   checkVulkanErrorOccured(inFlightFences[i], device->createFenceUnique(fenceCreateInfo), "", "Failed to create fence"))
			return false;
	}
	logger.logInfo("Created synchronization objects");

	return true;
}

RenderEngine::~RenderEngine()
{
	//Wait until rendering is finished before cleanup
	if(device)
		auto result = device->waitIdle();
}

bool RenderEngine::drawFrame()
{
	auto timeout = std::numeric_limits<uint64_t>::max();
	if(checkVulkanErrorOccured(device->waitForFences(inFlightFences[currentFrameIndex].get(), VK_TRUE, timeout), "", "Failed to wait for fence"))
		return false;

	auto [result, imageIndex] = device->acquireNextImageKHR(swapchainResources.swapchain.get(), timeout, imageAvailableSemaphores[currentFrameIndex].get(), {});
	if(result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR)
		return recreateSwapchain();
	else if(checkVulkanErrorOccured(result, "", "Failed to acquire next image"))
		return false;

	if(checkVulkanErrorOccured(device->resetFences(inFlightFences[currentFrameIndex].get()), "", "Failed to reset fence"))
		return false;

	if(checkVulkanErrorOccured(commandBuffers[currentFrameIndex].reset(), "", "Failed to reset command buffer"))
		return false;

	if(!recordCommandBuffer(commandBuffers[currentFrameIndex], imageIndex))
		return false;

	memcpy(quadDataBuffers[currentFrameIndex].data, quadPool.getData(), sizeof(QuadData) * quadPool.getSize());

	vk::PipelineStageFlags waitStage(vk::PipelineStageFlagBits::eColorAttachmentOutput);
	vk::SubmitInfo submitInfo(imageAvailableSemaphores[currentFrameIndex].get(), waitStage, commandBuffers[currentFrameIndex], renderFinishedSemaphores[currentFrameIndex].get());
	if(checkVulkanErrorOccured(graphicsQueue.submit(submitInfo, inFlightFences[currentFrameIndex].get()), "", "Failed to submit to graphics queue"))
		return false;

	vk::PresentInfoKHR presentInfo(renderFinishedSemaphores[currentFrameIndex].get(), swapchainResources.swapchain.get(), imageIndex);
	result = presentationQueue.presentKHR(presentInfo);
	if(result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR)
		return recreateSwapchain();
	else if(checkVulkanErrorOccured(result, "", "Failed to present image"))
		return false;

	currentFrameIndex = (currentFrameIndex + 1) % maxFramesInFlight;
	if(oldSwapchainResources.swapchain)
	{
		if(oldRendersRemaining == 0)
			oldSwapchainResources = {};
		else
			oldRendersRemaining--;
	}

	return true;
}

bool RenderEngine::recreateSwapchain()
{
	if(checkVulkanErrorOccured(physicalDeviceInfo.surfaceCapabilities, physicalDevice.getSurfaceCapabilitiesKHR(surface.get()), "", "Failed to get surface capabilities"))
		return false;

	if(physicalDeviceInfo.surfaceCapabilities.currentExtent.width == 0 || physicalDeviceInfo.surfaceCapabilities.currentExtent.height == 0)
		return true;

	if(checkVulkanErrorOccured(physicalDeviceInfo.surfaceFormats, physicalDevice.getSurfaceFormatsKHR(surface.get()), "", "Failed to get surface formats"))
		return false;

	if(checkVulkanErrorOccured(physicalDeviceInfo.presentModes, physicalDevice.getSurfacePresentModesKHR(surface.get()), "", "Failed to get surface present modes"))
		return false;

	oldSwapchainResources = std::move(swapchainResources);
	oldRendersRemaining = oldSwapchainResources.framebuffers.size();

	return swapchainResources.createSwapchain(*this);
}

bool RenderEngine::recordCommandBuffer(vk::CommandBuffer commandBuffer, uint32_t imageIndex) const
{
	vk::CommandBufferBeginInfo beginInfo;
	if(checkVulkanErrorOccured(commandBuffer.begin(beginInfo), "", "Failed to begin command buffer"))
		return false;

	vk::Rect2D renderArea({0, 0}, swapchainResources.imageExtent);
	vk::ClearValue clearValue(vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f));
	vk::RenderPassBeginInfo renderPassBeginInfo(swapchainResources.renderPass.get(), swapchainResources.framebuffers[imageIndex].get(), renderArea, clearValue);
	commandBuffer.beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);

	commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, graphicsPipeline.get());

	vk::Viewport viewport(0.0f, 0.0f, swapchainResources.imageExtent.width, swapchainResources.imageExtent.height, 0.0f, 1.0f);
	commandBuffer.setViewport(0, viewport);

	vk::Rect2D scissor({0, 0}, swapchainResources.imageExtent);
	commandBuffer.setScissor(0, scissor);

	PushConstantsBlock pushConstants{quadDataBuffers[currentFrameIndex].bufferAddress};
	commandBuffer.pushConstants<PushConstantsBlock>(pipelineLayout.get(), vk::ShaderStageFlagBits::eVertex, 0u, pushConstants);

	commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLayout.get(), 0, descriptorSets[currentFrameIndex], {});
	commandBuffer.draw(4, quadPool.getSize(), 0, 0);

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
		auto errorString = errorMessage.data() + ": "s + vk::to_string(result.result);
		logger.logError(errorString);
		return true;
	}
	else if(!successMessage.empty())
		logger.logInfo(successMessage);

	value = std::move(result.value);
	return false;
}

bool RenderEngine::checkVulkanErrorOccured(vk::Result result, std::string_view successMessage, std::string_view errorMessage) const
{
	if(result != vk::Result::eSuccess)
	{
		auto errorString = errorMessage.data() + ": "s + vk::to_string(result);
		logger.logError(errorString);
		return true;
	}
	else if(!successMessage.empty())
		logger.logInfo(successMessage);

	return false;
}

std::pair<int32_t, RenderEngine::PhysicalDeviceInfo> RenderEngine::getPhysicalDeviceInfo(vk::PhysicalDevice device, std::vector<char const*> const& requiredExtensions) const
{
	std::pair<int32_t, PhysicalDeviceInfo> result;
	result.first = -1;
	auto& [name, formats, presentModes, surfaceCapabilities, graphicsIndex, presentationIndex, deviceProperties, memoryProperties] = result.second;

	deviceProperties = device.getProperties();
	name = deviceProperties.deviceName.data();
	logger.logInfo(std::format("Checking {} suitability:", name));

	auto queueFamilyProperties = device.getQueueFamilyProperties();
	logger.logInfo(std::format("\t{} queue families available", queueFamilyProperties.size()));

	bool hasGraphicsQueueFamily{};
	bool hasPresentationQueueFamily{};
	for(size_t i{}; i < queueFamilyProperties.size(); i++)
	{
		if(queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eGraphics)
		{
			hasGraphicsQueueFamily = true;
			graphicsIndex = i;
			logger.logInfo(std::format("\tQueue family with index {} supports graphics", i));
		}

		vk::Bool32 surfaceSupport;
		if(checkVulkanErrorOccured(surfaceSupport, device.getSurfaceSupportKHR(i, surface.get()), "", "Failed to get surface support info"))
			return result;
		if(surfaceSupport)
		{
			hasPresentationQueueFamily = true;
			presentationIndex = i;
			logger.logInfo(std::format("\tQueue family with index {} supports presentation", i));
		}

		if(hasGraphicsQueueFamily && hasPresentationQueueFamily)
			break;
	}
	if(!hasGraphicsQueueFamily)
	{
		logger.logInfo("\tNo queue family with graphics support found");
		return result;
	}
	if(!hasPresentationQueueFamily)
	{
		logger.logInfo("\tNo queue family with presentation support found");
		return result;
	}

	std::vector<vk::ExtensionProperties> deviceExtensions;
	if(checkVulkanErrorOccured(deviceExtensions, device.enumerateDeviceExtensionProperties(), "", "Failed to enumerate physical device extension properties"))
		return result;
	logger.logInfo(std::format("\t{} physical device extensions available:", deviceExtensions.size()));
	std::unordered_set<std::string> extensionSet;
	for(auto extension : requiredExtensions)
		extensionSet.insert(extension);
	for(auto const& extension : deviceExtensions)
	{
		extensionSet.erase(extension.extensionName);
		logger.logInfo(std::format("\t\t{}", extension.extensionName.data()));
	}

	if(!extensionSet.empty())
	{
		logger.logInfo(std::format("\tRequired physical device extension {} not supported", *extensionSet.begin()));
		return result;
	}

	if(checkVulkanErrorOccured(formats, device.getSurfaceFormatsKHR(surface.get()), "", "\tFailed to get surface formats"))
		return result;

	if(checkVulkanErrorOccured(presentModes, device.getSurfacePresentModesKHR(surface.get()), "", "\tFailed to get surface present modes"))
		return result;

	if(checkVulkanErrorOccured(surfaceCapabilities, device.getSurfaceCapabilitiesKHR(surface.get()), "", "\tFailed to get surface capabilities"))
		return result;

	memoryProperties = device.getMemoryProperties();

	if(formats.empty() || presentModes.empty())
	{
		logger.logInfo("\tPhysical device doesn't support swapchain");
		return result;
	}

	vk::PhysicalDeviceVulkan12Features features12;
	vk::PhysicalDeviceVulkan11Features features11;
	features11.pNext = &features12;
	vk::PhysicalDeviceFeatures2 features({}, &features11);
	device.getFeatures2(&features);
	if(!features.features.shaderInt64)
	{
		logger.logInfo("\tPhysical device doesn't support 64 bit integers");
		return result;
	}
	if(!features.features.samplerAnisotropy)
	{
		logger.logInfo("\tPhysical device doesn't support anisotropic filtering");
		return result;
	}
	if(!features12.scalarBlockLayout)
	{
		logger.logInfo("\tPhysical device doesn't support scalar block layout");
		return result;
	}
	if(!features12.bufferDeviceAddress)
	{
		logger.logInfo("\tPhysical device doesn't support buffer device address");
		return result;
	}

	result.first = 0;
	if(deviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
		result.first++;
	logger.logInfo(std::format("\tPhysical device is a {}", vk::to_string(deviceProperties.deviceType)));

	return result;
}

int32_t RenderEngine::getMemoryType(vk::MemoryRequirements const& requirements, vk::MemoryPropertyFlags properties) const
{
	int32_t selectedMemoryType{-1};
	for(uint32_t i{0}; i < physicalDeviceInfo.memoryProperties.memoryTypeCount; i++)
	{
		if((requirements.memoryTypeBits & (1 << i)) && (physicalDeviceInfo.memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
		{
			selectedMemoryType = i;
			break;
		}
	}
	if(selectedMemoryType == -1)
		logger.logError("Failed to find suitable memory type for buffer");

	return selectedMemoryType;
}

vk::UniqueShaderModule RenderEngine::createShaderModule(std::string_view shaderFileName) const
{
	vk::UniqueShaderModule result;
	std::ifstream shaderFile(shaderFileName.data(), std::ios::ate | std::ios::binary | std::ios::in);

	if(!shaderFile)
	{
		logger.logError(std::format("Failed to open shader file {}", shaderFileName.data()));
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

template<class T>
bool RenderEngine::BufferResources<T>::createBuffer(RenderEngine const& engine, uint32_t size, vk::BufferUsageFlags usage)
{
	auto const& info = engine.physicalDeviceInfo;

	vk::BufferCreateInfo bufferCreateInfo({}, sizeof(T) * size, usage, vk::SharingMode::eExclusive, info.graphicsIndex);
	if(engine.checkVulkanErrorOccured(buffer, engine.device->createBufferUnique(bufferCreateInfo), "", "Failed to create buffer"))
		return false;
	auto memoryRequirements = engine.device->getBufferMemoryRequirements(buffer.get());

	vk::MemoryPropertyFlags memoryProperties{vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent};
	if(usage == vk::BufferUsageFlagBits::eShaderDeviceAddress)
		memoryProperties = {vk::MemoryPropertyFlagBits::eDeviceLocal | vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent};
	else if(usage == vk::BufferUsageFlagBits::eTransferSrc)
		memoryProperties = {vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent};

	auto selectedMemoryType = engine.getMemoryType(memoryRequirements, memoryProperties);
	if(selectedMemoryType == -1)
		return false;

	vk::MemoryAllocateFlagsInfo memoryAllocateFlagsInfo;
	if(usage == vk::BufferUsageFlagBits::eShaderDeviceAddress)
		memoryAllocateFlagsInfo = {vk::MemoryAllocateFlagBits::eDeviceAddress};

	vk::MemoryAllocateInfo memoryAllocateInfo(memoryRequirements.size, selectedMemoryType, &memoryAllocateFlagsInfo);
	if(engine.checkVulkanErrorOccured(bufferMemory, engine.device->allocateMemoryUnique(memoryAllocateInfo), "", "Failed to allocate buffer memory"))
		return false;

	if(engine.checkVulkanErrorOccured(engine.device->bindBufferMemory(buffer.get(), bufferMemory.get(), 0), "", "Failed to bind buffer memory"))
		return false;

	if(usage == vk::BufferUsageFlagBits::eShaderDeviceAddress)
	{
		vk::BufferDeviceAddressInfo deviceAddressInfo(buffer.get());
		bufferAddress = engine.device->getBufferAddress(deviceAddressInfo);
		if(!bufferAddress)
		{
			logger.logError("Failed to get buffer address");
			return false;
		}
	}

	if(engine.checkVulkanErrorOccured(data, engine.device->mapMemory(bufferMemory.get(), 0, bufferCreateInfo.size), "", "Failed to map buffer memory"))
		return false;

	return true;
}

bool RenderEngine::TextureResources::createTexture(RenderEngine const& engine, std::string_view filePath)
{
	auto tileImage = ImageLoader(filePath);
	vk::DeviceSize imageSize{(size_t)tileImage.width * tileImage.height * tileImage.channels};
	BufferResources<uint8_t> stagingBufferResources;
	if(!stagingBufferResources.createBuffer(engine, imageSize, vk::BufferUsageFlagBits::eTransferSrc))
		return false;

	memcpy(stagingBufferResources.data, tileImage.data, (size_t)imageSize);
	vk::ImageCreateInfo imageCreateInfo({}, vk::ImageType::e2D, vk::Format::eR8Unorm, vk::Extent3D{(uint32_t)tileImage.width, (uint32_t)tileImage.height, 1u},
										1, 1, vk::SampleCountFlagBits::e1, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
										vk::SharingMode::eExclusive, engine.physicalDeviceInfo.graphicsIndex, vk::ImageLayout::eUndefined);
	if(engine.checkVulkanErrorOccured(image, engine.device->createImageUnique(imageCreateInfo), "", "Failed to create texture image"))
		return false;

	auto memoryRequirements = engine.device->getImageMemoryRequirements(image.get());
	vk::MemoryAllocateInfo imageMemoryAllocateInfo(memoryRequirements.size, engine.getMemoryType(memoryRequirements, vk::MemoryPropertyFlagBits::eDeviceLocal));
	if(engine.checkVulkanErrorOccured(imageMemory, engine.device->allocateMemoryUnique(imageMemoryAllocateInfo), "", "Failed to allocate texture memory"))
		return false;

	if(engine.checkVulkanErrorOccured(engine.device->bindImageMemory(image.get(), imageMemory.get(), 0), "", "Failed to bind tile texture memory"))
		return false;

	{
		SingleUseCommandBuffer transitionCommandBuffer(engine, engine.graphicsQueue);

		vk::ImageSubresourceRange range(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
		vk::ImageMemoryBarrier memoryBarrier(vk::AccessFlagBits::eNone, vk::AccessFlagBits::eTransferWrite, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal,
											 VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, image.get(), range);
		transitionCommandBuffer->pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eTransfer, {}, {}, {}, memoryBarrier);
	}

	{
		SingleUseCommandBuffer copyCommandBuffer(engine, engine.graphicsQueue);

		vk::ImageSubresourceLayers imageSubresourceLayers(vk::ImageAspectFlagBits::eColor, 0, 0, 1);
		vk::BufferImageCopy imageCopy(0, 0, 0, imageSubresourceLayers, {}, {(uint32_t)tileImage.width, (uint32_t)tileImage.height, 1});
		copyCommandBuffer->copyBufferToImage(stagingBufferResources.buffer.get(), image.get(), vk::ImageLayout::eTransferDstOptimal, imageCopy);
	}

	{
		SingleUseCommandBuffer transitionCommandBuffer(engine, engine.graphicsQueue);

		vk::ImageSubresourceRange range(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
		vk::ImageMemoryBarrier memoryBarrier(vk::AccessFlagBits::eTransferWrite, vk::AccessFlagBits::eShaderRead, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal,
											 VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, image.get(), range);
		transitionCommandBuffer->pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader, {}, {}, {}, memoryBarrier);
	}

	vk::ImageSubresourceRange subresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
	vk::ImageViewCreateInfo viewCreateInfo({}, image.get(), vk::ImageViewType::e2D, vk::Format::eR8Unorm, {}, subresourceRange);
	if(engine.checkVulkanErrorOccured(imageView, engine.device->createImageViewUnique(viewCreateInfo), "", "Failed to create texture image view"))
		return false;

	vk::SamplerCreateInfo samplerCreateInfo({}, vk::Filter::eLinear, vk::Filter::eLinear, vk::SamplerMipmapMode::eLinear,
											vk::SamplerAddressMode::eRepeat, vk::SamplerAddressMode::eRepeat, vk::SamplerAddressMode::eRepeat,
											0.0f, VK_TRUE, engine.physicalDeviceInfo.properties.limits.maxSamplerAnisotropy, VK_FALSE, vk::CompareOp::eAlways,
											0.0f, 0.0f, vk::BorderColor::eIntOpaqueBlack, VK_FALSE);
	if(engine.checkVulkanErrorOccured(sampler, engine.device->createSamplerUnique(samplerCreateInfo), "", "Failed to create texture sampler"))
		return false;

	return true;
}

RenderEngine::SingleUseCommandBuffer::SingleUseCommandBuffer(RenderEngine const& engine, vk::Queue submitQueue):engine(engine), submitQueue(submitQueue)
{
	vk::CommandBufferAllocateInfo allocateInfo(engine.commandPool.get(), vk::CommandBufferLevel::ePrimary, 1);

	std::vector<vk::UniqueCommandBuffer> allocatedBuffers;
	if(engine.checkVulkanErrorOccured(allocatedBuffers, engine.device->allocateCommandBuffersUnique(allocateInfo), "", "Failed to allocate single use command buffer"))
		return;

	commandBuffer = std::move(allocatedBuffers[0]);

	vk::CommandBufferBeginInfo beginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
	if(engine.checkVulkanErrorOccured(commandBuffer->begin(beginInfo), "", "Failed to begin command buffer"))
		return;
}

RenderEngine::SingleUseCommandBuffer::~SingleUseCommandBuffer()
{
	if(engine.checkVulkanErrorOccured(commandBuffer->end(), "", "Failed to end command buffer"))
		return;

	vk::SubmitInfo submitInfo({}, {}, commandBuffer.get());
	if(engine.checkVulkanErrorOccured(submitQueue.submit(submitInfo), "", "Failed to submit command buffer"))
		return;

	if(engine.checkVulkanErrorOccured(submitQueue.waitIdle(), "", "Failed to wait after submitting command buffer"))
		return;
}

VKAPI_ATTR VkBool32 VKAPI_CALL RenderEngine::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
														   VkDebugUtilsMessageTypeFlagsEXT messageType,
														   const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
														   void* pUserData)
{
	if(messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
		logger.logInfo(std::format("{}", pCallbackData->pMessage));
	else if(messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
		logger.logError(std::format("{}", pCallbackData->pMessage));

	return VK_FALSE;
}