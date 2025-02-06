module;

#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#define VULKAN_HPP_NO_EXCEPTIONS
#include <vulkan/vulkan.hpp>

module RenderEngine;

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

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
	auto messengerCreateInfo{getDebugUtilsMessengerCreateInfo()};
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

	auto& swapExtent = physicalDeviceSurfaceCapabilities.currentExtent;
	if(swapExtent.width == std::numeric_limits<uint32_t>::max())
	{
		auto framebufferSize = window.getFramebufferSize();
		swapExtent.width = std::clamp(framebufferSize.first, physicalDeviceSurfaceCapabilities.minImageExtent.width, physicalDeviceSurfaceCapabilities.maxImageExtent.width);
		swapExtent.height = std::clamp(framebufferSize.second, physicalDeviceSurfaceCapabilities.minImageExtent.height, physicalDeviceSurfaceCapabilities.maxImageExtent.height);
	}
	Logger::logInfo(std::format("Swap extent is [{},{}]", swapExtent.width, swapExtent.height));

	uint32_t imageCount{physicalDeviceSurfaceCapabilities.minImageCount + 1};
	if(physicalDeviceSurfaceCapabilities.maxImageCount > 0 && imageCount > physicalDeviceSurfaceCapabilities.maxImageCount)
		imageCount = physicalDeviceSurfaceCapabilities.maxImageCount;
	Logger::logInfo(std::format("Image count is {}", imageCount));

	vk::SharingMode sharingMode = physicalDeviceInfo.graphicsIndex != physicalDeviceInfo.presentationIndex ? vk::SharingMode::eConcurrent : vk::SharingMode::eExclusive;
	std::vector<uint32_t> queueFamilyIndices{sharingMode == vk::SharingMode::eConcurrent ? std::vector{physicalDeviceInfo.graphicsIndex, physicalDeviceInfo.presentationIndex} : std::vector<uint32_t>{}};
	vk::SwapchainCreateInfoKHR swapchainCreateInfo({}, surface.surface, imageCount, selectedFormat.format, selectedFormat.colorSpace, swapExtent, 1, vk::ImageUsageFlagBits::eColorAttachment, sharingMode, queueFamilyIndices, physicalDeviceSurfaceCapabilities.currentTransform, vk::CompositeAlphaFlagBitsKHR::eOpaque, selectedPresentMode, VK_TRUE);
	if(checkVulkanErrorOccured(swapchain, device->createSwapchainKHRUnique(swapchainCreateInfo), "Created swapchain", "Failed to create swapchain"))
		return;
}

template<class Value, class Result>
bool RenderEngine::checkVulkanErrorOccured(Value& value, Result result, std::string_view successMessage, std::string_view errorMessage)
{
	if(result.result != vk::Result::eSuccess)
	{
		hasError = true;
		Logger::logError(errorMessage);
		return true;
	}
	else if(!successMessage.empty())
		Logger::logInfo(successMessage);

	value = std::move(result.value);
	return false;
}

std::pair<int32_t, RenderEngine::PhysicalDeviceInfo> RenderEngine::getPhysicalDeviceInfo(vk::PhysicalDevice device, std::vector<char const*> const& requiredExtensions)
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

vk::DebugUtilsMessengerCreateInfoEXT RenderEngine::getDebugUtilsMessengerCreateInfo() const
{
	return {{}, vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
			vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance | vk::DebugUtilsMessageTypeFlagBitsEXT::eDeviceAddressBinding, debugCallback};
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