module;

#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#define VULKAN_HPP_NO_EXCEPTIONS
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

export module RenderEngine;

export import RenderWindow;
export import Configuration;
export import Logger;

struct UniqueSurface
{
	UniqueSurface(vk::Instance& instance): instance(instance) {}
	~UniqueSurface() { instance.destroySurfaceKHR(surface);; }

	auto operator->() const { return surface; }
	operator vk::SurfaceKHR() const { return surface; }

	vk::Instance& instance;
	vk::SurfaceKHR surface;
};

export class RenderEngine
{
public:
	RenderEngine();

	auto getHasError() const { return hasError; }

private:
	template<class Value, class Result>
	bool checkVulkanErrorOccured(Value& value, Result result, std::string_view successMessage, std::string_view errorMessage);

	struct PhysicalDeviceInfo
	{
		std::string name;
		std::vector<vk::SurfaceFormatKHR> surfaceFormats;
		std::vector<vk::PresentModeKHR> presentModes;
		uint32_t graphicsIndex{}, presentationIndex{};
	};
	std::pair<int32_t, PhysicalDeviceInfo> getPhysicalDeviceInfo(vk::PhysicalDevice device, std::vector<char const*> const& requiredExtensions);

	vk::DebugUtilsMessengerCreateInfoEXT getDebugUtilsMessengerCreateInfo() const;
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
														VkDebugUtilsMessageTypeFlagsEXT messageType,
														const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
														void* pUserData);

	bool hasError{};

	RenderWindow window;
	vk::UniqueInstance instance;
	vk::UniqueDebugUtilsMessengerEXT debugMessenger;
	UniqueSurface surface;
	vk::PhysicalDevice physicalDevice;
	vk::UniqueDevice device;
	vk::Queue graphicsQueue;
	vk::Queue presentationQueue;
	vk::UniqueSwapchainKHR swapchain;
};