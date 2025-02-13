module;

#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#define VULKAN_HPP_NO_EXCEPTIONS
#define VULKAN_HPP_ASSERT_ON_RESULT
#include <vulkan/vulkan.hpp>

export module RenderEngine;

export import RenderWindow;
export import Configuration;
export import Logger;

export class RenderEngine
{
	struct PhysicalDeviceInfo
	{
		std::string name;
		std::vector<vk::SurfaceFormatKHR> surfaceFormats;
		std::vector<vk::PresentModeKHR> presentModes;
		vk::SurfaceCapabilitiesKHR surfaceCapabilities;
		uint32_t graphicsIndex{}, presentationIndex{};
	};

	struct SwapchainResources
	{
		SwapchainResources() = default;
		SwapchainResources(RenderEngine const& engine, PhysicalDeviceInfo const& info);

		vk::UniqueSwapchainKHR swapchain;
		std::vector<vk::Image> images;
		vk::Format imageFormat;
		vk::Extent2D imageExtent;
		std::vector<vk::UniqueImageView> imageViews;
	};

	struct SwapchainFramebuffers
	{
		SwapchainFramebuffers() = default;
		SwapchainFramebuffers(RenderEngine const& engine);

		std::vector<vk::UniqueFramebuffer> framebuffers;
	};

public:
	RenderEngine();
	~RenderEngine();

	bool drawFrame();

	auto getHasError() const { return hasError; }

private:
	bool recreateSwapchain();

	bool recordCommandBuffer(vk::CommandBuffer commandBuffer, uint32_t imageIndex) const;

	template<class Value, class Result>
	bool checkVulkanErrorOccured(Value& value, Result result, std::string_view successMessage, std::string_view errorMessage) const;
	bool checkVulkanErrorOccured(vk::Result result, std::string_view successMessage, std::string_view errorMessage) const;

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
	vk::UniqueSurfaceKHR surface;
	vk::PhysicalDevice physicalDevice;
	PhysicalDeviceInfo physicalDeviceInfo;
	vk::UniqueDevice device;
	vk::Queue graphicsQueue;
	vk::Queue presentationQueue;
	SwapchainResources swapchainResources;
	vk::UniquePipelineLayout pipelineLayout;
	vk::UniqueRenderPass renderPass;
	vk::UniquePipeline graphicsPipeline;
	SwapchainFramebuffers swapchainFramebuffers;
	vk::UniqueCommandPool commandPool;
	std::vector<vk::CommandBuffer> commandBuffers;

	static constexpr uint32_t maxFramesInFlight{2};
	std::array<vk::UniqueSemaphore, maxFramesInFlight> imageAvailableSemaphores;
	std::array<vk::UniqueSemaphore, maxFramesInFlight> renderFinishedSemaphores;
	std::array<vk::UniqueFence, maxFramesInFlight> inFlightFences;
	uint32_t currentFrameIndex{};
};
