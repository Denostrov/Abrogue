module;

#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#define VULKAN_HPP_NO_EXCEPTIONS
#define VULKAN_HPP_ASSERT_ON_RESULT
#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>

export module RenderEngine;

export import RenderWindow;
export import ObjectPools;
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
		vk::PhysicalDeviceMemoryProperties memoryProperties;
	};

	class SwapchainResources
	{
	public:
		SwapchainResources() = default;
		SwapchainResources(RenderEngine const& engine);

		vk::UniqueSwapchainKHR swapchain;
		std::vector<vk::Image> images;
		vk::Format imageFormat;
		vk::Extent2D imageExtent;
		std::vector<vk::UniqueImageView> imageViews;
		vk::UniqueRenderPass renderPass;
		std::vector<vk::UniqueFramebuffer> framebuffers;
	};

	template<class T>
	class BufferResources
	{
	public:
		BufferResources() = default;
		BufferResources(RenderEngine const& engine, uint32_t size, vk::BufferUsageFlags usage);

		vk::UniqueBuffer buffer;
		vk::UniqueDeviceMemory bufferMemory;
		vk::DeviceAddress bufferAddress;
		void* data{};
	};

	class SingleUseCommandBuffer
	{
	public:
		SingleUseCommandBuffer(RenderEngine const& engine, vk::Queue submitQueue);
		~SingleUseCommandBuffer();

		auto get() const { return commandBuffer.get(); }
		auto operator->() const { return commandBuffer.operator->(); }

	private:
		vk::UniqueCommandBuffer commandBuffer;
		
		RenderEngine const& engine;
		vk::Queue submitQueue;
	};

	struct PushConstantsBlock
	{
		vk::DeviceAddress quadReference;
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

	int32_t getMemoryType(vk::MemoryRequirements const& requirements, vk::MemoryPropertyFlags properties) const;

	vk::UniqueShaderModule createShaderModule(std::string_view shaderFileName) const;

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
														VkDebugUtilsMessageTypeFlagsEXT messageType,
														const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
														void* pUserData);

	mutable bool hasError{};

	static constexpr uint32_t maxFramesInFlight{2};

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
	vk::UniquePipeline graphicsPipeline;
	std::array<BufferResources<QuadData>, maxFramesInFlight> quadDataBuffers;
	vk::UniqueCommandPool commandPool;
	std::vector<vk::CommandBuffer> commandBuffers;
	vk::UniqueImage textureImage;
	vk::UniqueDeviceMemory textureImageMemory;

	std::array<vk::UniqueSemaphore, maxFramesInFlight> imageAvailableSemaphores;
	std::array<vk::UniqueSemaphore, maxFramesInFlight> renderFinishedSemaphores;
	std::array<vk::UniqueFence, maxFramesInFlight> inFlightFences;
	uint32_t currentFrameIndex{};

	uint32_t oldRendersRemaining{};
	SwapchainResources oldSwapchainResources;
};


