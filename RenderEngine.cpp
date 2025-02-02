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

	auto checkVulkanErrorOccured = [this](auto& value, auto result, std::string_view successMessage, std::string_view errorMessage)
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
	};

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

	for(auto layer : requiredLayers)
	{
		if(std::find_if(availableLayerProperties.begin(), availableLayerProperties.end(), [layer](auto const& property) { return std::strcmp(property.layerName, layer) == 0; }) != availableLayerProperties.end())
			continue;

		hasError = true;
		Logger::logError(std::format("Required layer {} not supported", layer));
		return;
	}

	auto requiredExtensions = window.getRequiredExtensions();
	if constexpr(isDebugBuild)
		requiredExtensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	Logger::logInfo(std::format("{} Vulkan instance extensions required:", requiredExtensions.size()));
	for(auto extension : requiredExtensions)
		Logger::logInfo(std::format("\t{}", extension));

	for(auto extension : requiredExtensions)
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
		instanceCreateInfo = vk::InstanceCreateInfo({}, &applicationInfo, requiredLayers, requiredExtensions, &messengerCreateInfo);
	else
		instanceCreateInfo = vk::InstanceCreateInfo({}, &applicationInfo, requiredLayers, requiredExtensions);
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

	Logger::logInfo(std::format("{} physical devices available:", availablePhysicalDevices.size()));
	for(auto availableDevice : availablePhysicalDevices)
		Logger::logInfo(std::format("\t{}", availableDevice.getProperties().deviceName.data()));

	auto maxDeviceScore{0};
	std::string physicalDeviceName;
	uint32_t graphicsQueueFamilyIndex{};
	uint32_t presentationQueueFamilyIndex{};
	for(auto availableDevice : availablePhysicalDevices)
	{
		auto deviceProperties = availableDevice.getProperties();
		Logger::logInfo(std::format("Checking {} suitability:", deviceProperties.deviceName.data()));

		auto queueFamilyProperties = availableDevice.getQueueFamilyProperties();
		Logger::logInfo(std::format("\t{} queue families available", queueFamilyProperties.size()));

		bool hasGraphicsQueueFamily{};
		bool hasPresentationQueueFamily{};
		for(size_t i{}; i < queueFamilyProperties.size(); i++)
		{
			if(queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eGraphics)
			{
				hasGraphicsQueueFamily = true;
				graphicsQueueFamilyIndex = i;
				Logger::logInfo(std::format("\tQueue family with index {} supports graphics", i));
			}

			vk::Bool32 surfaceSupport;
			if(checkVulkanErrorOccured(surfaceSupport, availableDevice.getSurfaceSupportKHR(i, surface), "", "Failed to get surface support info"))
			   return;
			if(surfaceSupport)
			{
				hasPresentationQueueFamily = true;
				presentationQueueFamilyIndex = i;
				Logger::logInfo(std::format("\tQueue family with index {} supports presentation", i));
			}

			if(hasGraphicsQueueFamily && hasPresentationQueueFamily)
				break;
		}
		if(!hasPresentationQueueFamily || !hasGraphicsQueueFamily)
		{
			Logger::logInfo("\tNo queue families with graphics support found");
			continue;
		}

		auto deviceScore{0};
		if(deviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
		{
			deviceScore++;
			Logger::logInfo("\tIs a discrete GPU");
		}
		else
			Logger::logInfo("\tIs not a discrete GPU");

		if(deviceScore > maxDeviceScore)
		{
			maxDeviceScore = deviceScore;
			physicalDevice = availableDevice;
			physicalDeviceName = deviceProperties.deviceName.data();
		}
	}

	if(!physicalDevice)
	{
		hasError = true;
		Logger::logError("No suitable physical devices found. Try updating drivers");
		return;
	}
	Logger::logInfo(std::format("Picked {} as a suitable physical device", physicalDeviceName.data()));

	std::unordered_set uniqueQueueFamilyIndices{graphicsQueueFamilyIndex, presentationQueueFamilyIndex};
	std::array<float, 1> queuePriorities{1.0f};
	std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
	queueCreateInfos.reserve(uniqueQueueFamilyIndices.size());
	for(auto index : uniqueQueueFamilyIndices)
		queueCreateInfos.emplace_back(vk::DeviceQueueCreateInfo{{}, index, queuePriorities});
	
	vk::DeviceCreateInfo deviceCreateInfo({}, queueCreateInfos, requiredLayers);
	checkVulkanErrorOccured(device, physicalDevice.createDeviceUnique(deviceCreateInfo), "Created logical device", "Failed to create logical device");

	graphicsQueue = device->getQueue(graphicsQueueFamilyIndex, 0);
	presentationQueue = device->getQueue(presentationQueueFamilyIndex, 0);
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