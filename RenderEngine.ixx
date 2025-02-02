module;

#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#define VULKAN_HPP_NO_EXCEPTIONS
#include <vulkan/vulkan.hpp>

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
};