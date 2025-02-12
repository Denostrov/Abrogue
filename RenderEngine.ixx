module;

#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#define VULKAN_HPP_NO_EXCEPTIONS
#define VULKAN_HPP_ASSERT_ON_RESULT
#include <vulkan/vulkan.hpp>

export module RenderEngine;

export import RenderWindow;
export import Configuration;
export import Logger;

struct UniqueSurface
{
	UniqueSurface() {}
	~UniqueSurface() { if(instance && surface) instance.destroySurfaceKHR(surface); }

	auto operator->() const { return surface; }
	operator vk::SurfaceKHR() const { return surface; }

	vk::Instance instance;
	vk::SurfaceKHR surface;
};

export class RenderEngine
{
public:
	RenderEngine();
	~RenderEngine();

	bool drawFrame();

	auto getHasError() const { return hasError; }

private:
	bool recordCommandBuffer(vk::CommandBuffer commandBuffer, uint32_t imageIndex) const;

	template<class Value, class Result>
	bool checkVulkanErrorOccured(Value& value, Result result, std::string_view successMessage, std::string_view errorMessage) const;
	bool checkVulkanErrorOccured(vk::Result result, std::string_view successMessage, std::string_view errorMessage) const;

	struct PhysicalDeviceInfo
	{
		std::string name;
		std::vector<vk::SurfaceFormatKHR> surfaceFormats;
		std::vector<vk::PresentModeKHR> presentModes;
		uint32_t graphicsIndex{}, presentationIndex{};
	};
	std::pair<int32_t, PhysicalDeviceInfo> getPhysicalDeviceInfo(vk::PhysicalDevice device, std::vector<char const*> const& requiredExtensions) const;

	vk::UniqueShaderModule createShaderModule(std::string_view shaderFileName) const;

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
														VkDebugUtilsMessageTypeFlagsEXT messageType,
														const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
														void* pUserData);

	mutable bool hasError{};

	RenderWindow window;
	vk::UniqueInstance instance;
	vk::UniqueDebugUtilsMessengerEXT debugMessenger;
	UniqueSurface surface;
	vk::PhysicalDevice physicalDevice;
	vk::UniqueDevice device;
	vk::Queue graphicsQueue;
	vk::Queue presentationQueue;
	vk::UniqueSwapchainKHR swapchain;
	std::vector<vk::Image> swapchainImages;
	vk::Format swapchainImageFormat;
	vk::Extent2D swapchainImageExtent;
	std::vector<vk::UniqueImageView> swapchainImageViews;
	vk::UniquePipelineLayout pipelineLayout;
	vk::UniqueRenderPass renderPass;
	vk::UniquePipeline graphicsPipeline;
	std::vector<vk::UniqueFramebuffer> swapchainFramebuffers;
	vk::UniqueCommandPool commandPool;
	std::vector<vk::CommandBuffer> commandBuffers;

	static constexpr uint32_t maxFramesInFlight{2};
	std::array<vk::UniqueSemaphore, maxFramesInFlight> imageAvailableSemaphores;
	std::array<vk::UniqueSemaphore, maxFramesInFlight> renderFinishedSemaphores;
	std::array<vk::UniqueFence, maxFramesInFlight> inFlightFences;
	uint32_t currentFrameIndex{};
};