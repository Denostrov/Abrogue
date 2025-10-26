module;

#include "vulkan/vk_platform.h"

module Abrogue:RenderEngine;

import :QuadPool;

import vulkan_hpp;

/*
 * RenderEngine - class for handling Vulkan resources
 */
class RenderEngine
{
    //Struct for storing physical device properties
    struct PhysicalDeviceInfo
    {
        FixedString<256> name;
        std::vector<vk::SurfaceFormatKHR> surfaceFormats;
        std::vector<vk::PresentModeKHR> presentModes;
        vk::SurfaceCapabilitiesKHR surfaceCapabilities;
        std::uint32_t graphicsIndex{}, presentationIndex{};
        vk::PhysicalDeviceProperties properties;
        vk::PhysicalDeviceMemoryProperties memoryProperties;
    };

    //Class for handling swapchain creation
    class SwapchainResources
    {
    public:
        SwapchainResources() = default;

        SwapchainResources(SwapchainResources&& rhs) noexcept { *this = std::move(rhs); }
        SwapchainResources& operator=(SwapchainResources&& rhs) noexcept;

        //Initialize swapchain resources
        [[nodiscard]] bool createSwapchain(RenderEngine const& engine);

        vk::UniqueSwapchainKHR swapchain;
        std::vector<vk::Image> images;
        vk::Format imageFormat{};
        vk::Extent2D imageExtent;
        std::vector<vk::UniqueImageView> imageViews;
        vk::UniqueRenderPass renderPass;
        std::vector<vk::UniqueFramebuffer> framebuffers;
    };

    //Class for handling buffer creation
    template <class T>
    class BufferResources
    {
    public:
        BufferResources() = default;

        //Initialize buffer resources
        [[nodiscard]] bool createBuffer(RenderEngine const& engine, std::uint32_t size, vk::BufferUsageFlags usage);

        vk::UniqueBuffer buffer;
        vk::UniqueDeviceMemory bufferMemory;
        vk::DeviceAddress bufferAddress{};
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
        explicit SingleUseCommandBuffer(vk::Queue submitQueue);
        //Submit command buffer upon deletion
        ~SingleUseCommandBuffer();

        vk::Queue submitQueue;
        vk::UniqueCommandBuffer commandBuffer;
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
    [[nodiscard]] bool init();

    //Submit drawing commands for current frame
    [[nodiscard]] bool drawFrame();

private:
    template <class Value, class ResultValue>
    [[nodiscard]] bool checkError(Value& value, ResultValue resultValue, std::string_view successMessage, std::string_view errorMessage) const;
    [[nodiscard]] static bool checkError(vk::Result result, std::string_view successMessage, std::string_view errorMessage);

    template<std::size_t currentIndex = 0>
    std::size_t copyQuadPools(std::size_t instanceCount = 0);

    //Create new swapchain and mark old one for deletion
    [[nodiscard]] bool recreateSwapchain();

    //Record drawing commands to buffer
    [[nodiscard]] bool recordCommandBuffer(vk::CommandBuffer commandBuffer, std::uint32_t imageIndex, std::size_t instanceCount) const;

    //Get physical device suitability score and its properties(-1 score - device not suitable)
    [[nodiscard]] std::pair<std::int32_t, PhysicalDeviceInfo> getPhysicalDeviceInfo(vk::PhysicalDevice device,
                                                                                    std::vector<char const*> const& requiredExtensions) const;

    //Get index of memory type that fits requirements(-1 index - no fitting type found)
    [[nodiscard]] std::int32_t getMemoryType(vk::MemoryRequirements const& requirements, vk::MemoryPropertyFlags properties) const;

    //Create shader from file
    [[nodiscard]] vk::UniqueShaderModule createShaderModule(std::string_view shaderFileName) const;

    //Callback for debug utils messenger
    static VKAPI_ATTR vk::Bool32 VKAPI_CALL debugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                          vk::DebugUtilsMessageTypeFlagsEXT messageType,
                                                          vk::DebugUtilsMessengerCallbackDataEXT const* pCallbackData,
                                                          void* pUserData);

    static constexpr std::uint32_t maxFramesInFlight{2};

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

    FixedVector<vk::UniqueSemaphore, maxFramesInFlight> imageAcquiredSemaphores;
    FixedVector<vk::UniqueSemaphore, 8> submitFinishedSemaphores;
    FixedVector<vk::UniqueFence, maxFramesInFlight> inFlightFences;
    std::uint32_t currentFrameIndex{};

    std::uint32_t oldRendersRemaining{};
    SwapchainResources oldSwapchainResources;
};
inline RenderEngine renderEngine;