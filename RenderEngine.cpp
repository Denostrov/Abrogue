module;

#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#define VULKAN_HPP_NO_EXCEPTIONS
#include <vulkan/vulkan.hpp>

module RenderEngine;

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

RenderEngine::RenderEngine()
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
	for(uint32_t i{}; i < requiredLayers.size(); i++)
		Logger::logInfo(std::format("\t{}", requiredLayers[i]));

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
	for(uint32_t i{}; i < requiredExtensions.size(); i++)
		Logger::logInfo(std::format("\t{}", requiredExtensions[i]));

	for(auto extension : requiredExtensions)
	{
		if(std::find_if(availableExtensionProperties.begin(), availableExtensionProperties.end(), [extension](auto const& property) { return std::strcmp(property.extensionName, extension) == 0; }) != availableExtensionProperties.end())
			continue;

		hasError = true;
		Logger::logError(std::format("Required extension {} not supported", extension));
		return;
	}
	auto instanceCreateInfo = vk::InstanceCreateInfo({}, &applicationInfo, requiredLayers, requiredExtensions);
	if(checkVulkanErrorOccured(instance, vk::createInstanceUnique(instanceCreateInfo), "Created Vulkan instance", "Failed to create Vulkan instance"))
		return;

	VULKAN_HPP_DEFAULT_DISPATCHER.init(instance.get());

	if constexpr(isDebugBuild)
	{
		vk::DebugUtilsMessengerCreateInfoEXT messengerCreateInfo({}, vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
																 vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance | vk::DebugUtilsMessageTypeFlagBitsEXT::eDeviceAddressBinding, debugCallback);
		if(checkVulkanErrorOccured(debugMessenger, instance->createDebugUtilsMessengerEXTUnique(messengerCreateInfo), "Created debug messenger", "Failed to create debug messenger"))
		   return;
	}
}

VKAPI_ATTR VkBool32 VKAPI_CALL RenderEngine::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
											 VkDebugUtilsMessageTypeFlagsEXT messageType,
											 const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
											 void* pUserData)
{
	if(messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
		Logger::logInfo(std::format("Validation layer: {}", pCallbackData->pMessage));
	else if(messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
		Logger::logError(std::format("Validation layer: {}", pCallbackData->pMessage));

	return VK_FALSE;
}