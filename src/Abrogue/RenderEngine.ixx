module;

#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#define VULKAN_HPP_NO_EXCEPTIONS
#define VULKAN_HPP_ASSERT_ON_RESULT
#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>

export module RenderEngine;

export import RenderWindow;
export import QuadPool;
export import Configuration;
export import Logger;

//Class for handling Vulkan resources
export class RenderEngine
{
	//Struct for storing physical device properties
	struct PhysicalDeviceInfo
	{
		std::string name;
		std::vector<vk::SurfaceFormatKHR> surfaceFormats;
		std::vector<vk::PresentModeKHR> presentModes;
		vk::SurfaceCapabilitiesKHR surfaceCapabilities;
		uint32_t graphicsIndex{}, presentationIndex{};
		vk::PhysicalDeviceProperties properties;
		vk::PhysicalDeviceMemoryProperties memoryProperties;
	};

	//Class for handling swapchain creation
	class SwapchainResources
	{
	public:
		SwapchainResources() = default;

		SwapchainResources(SwapchainResources&& rhs) { *this = std::move(rhs); }
		SwapchainResources& operator=(SwapchainResources&& rhs);

		//Initialize swapchain resources
		[[nodiscard]] bool createSwapchain(RenderEngine const& engine);

		vk::UniqueSwapchainKHR swapchain;
		std::vector<vk::Image> images;
		vk::Format imageFormat;
		vk::Extent2D imageExtent;
		std::vector<vk::UniqueImageView> imageViews;
		vk::UniqueRenderPass renderPass;
		std::vector<vk::UniqueFramebuffer> framebuffers;
	};

	//Class for handling buffer creation
	template<class T>
	class BufferResources
	{
	public:
		BufferResources() = default;

		//Initialize buffer resources
		[[nodiscard]] bool createBuffer(RenderEngine const& engine, uint32_t size, vk::BufferUsageFlags usage);

		vk::UniqueBuffer buffer;
		vk::UniqueDeviceMemory bufferMemory;
		vk::DeviceAddress bufferAddress;
		void* data{};
	};

	//Class for handling texture creation
	class TextureResources
	{
	public:
		TextureResources() = default;

		//Initialize texture resources
		[[nodiscard]] bool createTexture(RenderEngine const& engine, std::string_view filePath);

		vk::UniqueImage image;
		vk::UniqueDeviceMemory imageMemory;
		vk::UniqueImageView imageView;
		vk::UniqueSampler sampler;
	};

	//Class for creating and submitting one time command buffers
	class SingleUseCommandBuffer
	{
	public:
		//Create command buffer resources
		SingleUseCommandBuffer(RenderEngine const& engine, vk::Queue submitQueue);
		//Submit command buffer upon deletion
		~SingleUseCommandBuffer();

		auto get() const { return commandBuffer.get(); }
		auto operator->() const { return commandBuffer.operator->(); }

	private:
		vk::UniqueCommandBuffer commandBuffer;
		
		RenderEngine const& engine;
		vk::Queue submitQueue;
	};

	//Struct for shader constants
	struct PushConstantsBlock
	{
		vk::DeviceAddress quadReference;
	};

public:
	RenderEngine() = default;
	//Wait until rendering is finished and cleanup
	~RenderEngine();

	//Initialize Vulkan resources
	[[nodiscard]] bool initVulkan();

	//Submit drawing commands for current frame
	[[nodiscard]] bool drawFrame();

	[[nodiscard]] auto getFramebufferSize() const { return window.getWindowSize(); };

private:
	//Create new swapchain and mark old one for deletion
	[[nodiscard]] bool recreateSwapchain();

	//Record drawing commands to buffer
	[[nodiscard]] bool recordCommandBuffer(vk::CommandBuffer commandBuffer, uint32_t imageIndex) const;

	//Get physical device suitability score and its properties(-1 score - device not suitable)
	[[nodiscard]] std::pair<int32_t, PhysicalDeviceInfo> getPhysicalDeviceInfo(vk::PhysicalDevice device, std::vector<char const*> const& requiredExtensions) const;

	//Get index of memory type that fits requirements(-1 index - no fitting type found)
	[[nodiscard]] int32_t getMemoryType(vk::MemoryRequirements const& requirements, vk::MemoryPropertyFlags properties) const;

	//Create shader from file
	[[nodiscard]] vk::UniqueShaderModule createShaderModule(std::string_view shaderFileName) const;

	//Callback for debug utils messenger
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
														VkDebugUtilsMessageTypeFlagsEXT messageType,
														const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
														void* pUserData);

	static constexpr uint32_t maxFramesInFlight{2};

	RenderWindow window;
	vk::detail::DynamicLoader dynamicLoader;
	vk::UniqueInstance instance;
	vk::UniqueDebugUtilsMessengerEXT debugMessenger;
	vk::UniqueSurfaceKHR surface;
	vk::PhysicalDevice physicalDevice;
	PhysicalDeviceInfo physicalDeviceInfo;
	vk::UniqueDevice device;
	vk::Queue graphicsQueue;
	vk::Queue presentationQueue;
	SwapchainResources swapchainResources;
	vk::UniqueCommandPool commandPool;
	TextureResources textureResources;
	vk::UniqueDescriptorSetLayout descriptorSetLayout;
	vk::UniqueDescriptorPool descriptorPool;
	std::array<vk::DescriptorSet, maxFramesInFlight> descriptorSets;
	vk::UniquePipelineLayout pipelineLayout;
	vk::UniquePipeline graphicsPipeline;
	std::array<BufferResources<QuadData>, maxFramesInFlight> quadDataBuffers;
	std::array<vk::CommandBuffer, maxFramesInFlight> commandBuffers;

	std::array<vk::UniqueSemaphore, maxFramesInFlight> imageAvailableSemaphores;
	std::array<vk::UniqueSemaphore, maxFramesInFlight> renderFinishedSemaphores;
	std::array<vk::UniqueFence, maxFramesInFlight> inFlightFences;
	uint32_t currentFrameIndex{};

	uint32_t oldRendersRemaining{};
	SwapchainResources oldSwapchainResources;
};

export inline RenderEngine renderEngine;
