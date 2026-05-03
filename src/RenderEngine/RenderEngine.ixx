module;

#include <vulkan/vulkan_hpp_macros.hpp>
#include "vulkan/vk_platform.h"

export module RenderEngine;

import Helpers;
import ImageLoader;

import vulkan_hpp;
import GLM;

import std;

using namespace std::literals;

/*
 * Class for storing rendering data of a single quad
 */
export class QuadData
{
public:
    static constexpr float tileScaleX = 0.5f / 36.0f;
    static constexpr float tileScaleY = 1.0f / 36.0f;

    QuadData() = default;
    //Create Data with position in world coordinates and color
    QuadData(glm::vec2 position, glm::uvec2 colors, std::uint32_t glyph, glm::vec2 scale = {1.0, 1.0})
        : position(position.x * tileScaleX, position.y * tileScaleY),
          scale(scale.x * tileScaleX, scale.y * tileScaleY),
          colors(colors), glyph(glyph)
    {}

    //Set position in world coordinates
    void setPosition(float positionX, float positionY) { position = {positionX * tileScaleX, positionY * tileScaleY}; }
    //Set scale in world coordinates
    void setScale(float scaleX, float scaleY) { scale = {tileScaleX * scaleX, tileScaleY * scaleY}; }
    //Set quad rotaion in radians
    void setRotation(float angle) { rotation = {std::cos(angle), std::sin(angle)}; }
    //Set quad rotation in precalculated cos and sin
    void setRotation(float cos, float sin) { rotation = {cos, sin}; }
    //Set packed glyph color
    void setColor(PackedColor packedColor) { colors[0] = packedColor; }
    //Set packed background color
    void setBackgroundColor(PackedColor packedColor) { colors[1] = packedColor; }
    //Set glyph index
    void setGlyph(std::uint32_t newGlyph) { glyph = newGlyph; }

private:
    glm::vec2 position{};           //Position in screenspace coordinates
    glm::vec2 scale{};              //Scale in screenspace coordinates
    glm::vec2 rotation{1.0f, 0.0f}; //Cos and Sin of rotation angle
    glm::uvec2 colors{};            //Packed glyph and background colors
    std::uint32_t glyph{};          //Index of drawn symbol
};

//Layers for defining draw order
export enum class QuadLayer
{
    eMap,
    eItem,
    eEntity,
    eMapOverlay,
    ePopupBackground,
    ePopup,
    COUNT
};

/*
 * Class for a bidirectional reference to quad data
 */
export template <QuadLayer>
class QuadReference
{
public:
    QuadReference() = default;

    //Swap reference indices and pointers in the pool
    QuadReference(QuadReference&& rhs) noexcept { *this = std::move(rhs); }
    QuadReference& operator=(QuadReference&& rhs) noexcept;

    //Destroy a reference to quad if it exists
    ~QuadReference();

    //Create a reference if it doesn't exist and set its data
    template <IsSameType<QuadData> T>
    void init(T&& quadData);
    //Destroy a reference if it exists
    void clear();

    operator bool() const { return index != -1; }

    //Modify an existing reference
    void setPosition(float positionX, float positionY) const;
    void setGlyph(std::uint8_t glyph) const;
    void setColor(std::uint32_t packedColor) const;
    void setBackgroundColor(std::uint32_t packedColor) const;

private:
    std::int64_t index{-1}; //Index of quad data in the pool
};

/*
 * Class for handling quad data storage
 */
export class QuadPool
{
    static constexpr Array<std::size_t, QuadLayer::COUNT> capacities{8192uz, 512uz, 512uz, 512uz, 512uz, 2048uz};

    template <QuadLayer layer>
    struct Storage
    {
        FixedVector<QuadData, capacities[layer]> data;
        FixedVector<QuadReference<layer>*, capacities[layer]> references;
    };

public:
    QuadPool() = default;

    template <QuadLayer layer>
    [[nodiscard]] auto getData() { return getStorage<layer>().data.getSpan(); }
    [[nodiscard]] static constexpr auto getCapacity() { return std::accumulate(capacities.begin(), capacities.end(), 0uz); }

private:
    template <QuadLayer layer>
    [[nodiscard]] auto&& getStorage() { return std::get<(std::size_t)layer>(storage); }

    EnumTupleType<QuadLayer, Storage> storage;

    template <QuadLayer layer>
    friend class QuadReference;
};
export inline QuadPool quadPool;

/*
 * RenderWindow - class for handling SDL initialization and window creation
 */
export class RenderWindow
{
public:
    enum class State
    {
        eWindowed,
        eMaximized,
        eFullscreen
    };

    RenderWindow() = default;
    ~RenderWindow()
    {
        if (window)
            SDL_DestroyWindow(window);

        if (SDL_WasInit(sdlInitVideo))
            SDL_QuitSubSystem(sdlInitVideo);

        SDL_Quit();
    }

    // Initialize SDL and create a window
    [[nodiscard]] bool init(std::string_view name, std::string_view version, std::string_view identifier, std::string_view creator, std::string_view copyright,
                            std::string_view type, std::int64_t width, std::int64_t height, State state)
    {
        // Set application metadata
        FixedString<256> fullAppName;
        fullAppName.format("{} {}", name, version);
        if (checkError(SDL_SetAppMetadataProperty(sdlPropAppMetadataNameString, fullAppName.getData()) &&
                       SDL_SetAppMetadataProperty(sdlPropAppMetadataVersionString, version.data()) &&
                       SDL_SetAppMetadataProperty(sdlPropAppMetadataIdentifierString, identifier.data()) &&
                       SDL_SetAppMetadataProperty(sdlPropAppMetadataCreatorString, creator.data()) &&
                       SDL_SetAppMetadataProperty(sdlPropAppMetadataCopyrightString, copyright.data()) &&
                       SDL_SetAppMetadataProperty(sdlPropAppMetadataTypeString, type.data())))
            return false;

        // Initialize SDL video system
        if (checkError(SDL_Init(sdlInitVideo)))
            return false;

        int displayCount{};
        auto displays = SDL_GetDisplays(&displayCount);
        if (checkError(displays))
            return false;

        SDL_Rect displayUsableRect{};
        if (checkError(SDL_GetDisplayUsableBounds(displays[0], &displayUsableRect)))
            return false;
        SDL_free(displays);

        auto windowWidth = std::min((std::int64_t)displayUsableRect.w, width);
        auto windowHeight = std::min((std::int64_t)displayUsableRect.h, height);

        // Create window
        window = SDL_CreateWindow(fullAppName.getData(), windowWidth, windowHeight,
                                  sdlWindowVulkan | sdlWindowResizable |
                                      (state == State::eFullscreen      ? sdlWindowFullscreen
                                           : state == State::eMaximized ? sdlWindowMaximized
                                                                        : 0));
        if (checkError(window))
            return false;

        auto usableCenterX = displayUsableRect.x + displayUsableRect.w / 2;
        auto usableCenterY = displayUsableRect.y + displayUsableRect.h / 2;
        if (auto topLeftX = usableCenterX - windowWidth / 2, topLeftY = usableCenterY - windowHeight / 2;
            checkError(SDL_SetWindowPosition(window, topLeftX, topLeftY)))
            return false;

        if (checkError(SDL_SyncWindow(window)))
            return false;

        // Get required extensions
        std::uint32_t extensionCount{};
        auto vulkanExtensions = SDL_Vulkan_GetInstanceExtensions(&extensionCount);
        if (checkError(vulkanExtensions))
            return false;

        requiredExtensions = {vulkanExtensions, extensionCount};

        return true;
    }

    // Create Vulkan surface from window
    [[nodiscard]] VkSurfaceKHR createSurface(VkInstance instance) const
    {
        VkSurfaceKHR result;
        if (checkError(SDL_Vulkan_CreateSurface(window, instance, nullptr, &result)))
            return VkSurfaceKHR{};

        return result;
    }

    // Get the window size in pixels
    [[nodiscard]] std::pair<std::int64_t, std::int64_t> getWindowSize() const
    {
        int width{}, height{};
        SDL_GetWindowSizeInPixels(window, &width, &height);
        return {width, height};
    }

    [[nodiscard]] auto getRequiredExtensions() const { return requiredExtensions; }
    [[nodiscard]] bool getIsMaximized() const { return SDL_GetWindowFlags(window) & sdlWindowMaximized; }
    [[nodiscard]] bool getIsFullscreen() const { return SDL_GetWindowFlags(window) & sdlWindowFullscreen; }

    // Maximize the window and sync changes
    void setIsMaximized(bool maximized) const
    {
        maximized ? SDL_MaximizeWindow(window) : SDL_RestoreWindow(window);
        SDL_SyncWindow(window);
    }
    // Fullscreen the window and sync changes
    void setIsFullscreen(bool fullscreen) const
    {
        SDL_SetWindowFullscreen(window, fullscreen);
        SDL_SyncWindow(window);
    }

private:
    [[nodiscard]] static bool checkError(bool value)
    {
        if (value)
            return false;

        if (auto errorString = SDL_GetError(); errorString[0] != '\0')
        {
            logger.logError("{}", errorString);
            SDL_ClearError();
        }

        return true;
    }

    SDL_Window* window{};
    std::span<char const* const> requiredExtensions{};
};
export inline RenderWindow renderWindow;

/*
 * Class for handling Vulkan resources
 */
export class RenderEngine
{
    // Struct for storing physical device properties
    struct PhysicalDeviceInfo
    {
        [[nodiscard]] std::int32_t getMemoryTypeIndex(vk::MemoryRequirements const& requirements, vk::MemoryPropertyFlags properties) const
        {
            std::int32_t selectedMemoryType{-1};
            for (std::uint32_t i{}; i < memoryProperties.memoryTypeCount; i++)
            {
                if (requirements.memoryTypeBits & 1 << i && (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
                {
                    selectedMemoryType = i;
                    break;
                }
            }
            if (selectedMemoryType == -1)
                logger.logError("Failed to find suitable memory type for buffer");

            return selectedMemoryType;
        }

        FixedString<256> name;
        std::uint32_t surfaceFormatCount{};
        Array<vk::SurfaceFormatKHR, 16> surfaceFormats;
        std::uint32_t presentModeCount{};
        Array<vk::PresentModeKHR, 16> presentModes;
        vk::SurfaceCapabilitiesKHR surfaceCapabilities;
        std::uint32_t graphicsIndex{}, presentationIndex{};
        float maxSamplerAnisotropy{};
        vk::PhysicalDeviceMemoryProperties memoryProperties;
    };

    // Struct for storing swapchain resources
    struct SwapchainResources
    {
        vk::SwapchainKHR swapchain;
        std::uint32_t imageCount{};
        Array<vk::Image, 16> images;
        vk::Format imageFormat{};
        vk::Extent2D imageExtent;
        Array<vk::ImageView, 16> imageViews;
        vk::RenderPass renderPass;
        Array<vk::Framebuffer, 16> framebuffers;
    };

    // Struct for storing buffer resources
    struct BufferResources
    {
        vk::Buffer buffer;
        vk::DeviceMemory bufferMemory;
        vk::DeviceAddress bufferAddress{};
        void* data{};
    };

    class ShaderModule
    {
    public:
        ShaderModule() = default;
        ~ShaderModule()
        {
            if (module)
                device.destroyShaderModule(module);
        }

        [[nodiscard]] bool init(vk::Device curDevice, std::string_view fileName)
        {
            device = curDevice;

            std::ifstream shaderFile(fileName.data(), std::ios::ate | std::ios::binary | std::ios::in);

            if (!shaderFile)
            {
                logger.logError("Failed to open shader file {}", fileName.data());
                return false;
            }

            auto fileSize{shaderFile.tellg()};
            Array<std::uint32_t, 4096> fileBuffer;
            shaderFile.seekg(0);
            shaderFile.read(reinterpret_cast<char*>(fileBuffer.getData()), fileSize);

            vk::ShaderModuleCreateInfo createInfo({}, fileSize, fileBuffer.getData());
            if (checkError(module, device.createShaderModule(createInfo), "Created shader module", "Failed to create shader module"))
                return false;

            return true;
        }

        [[nodiscard]] auto getModule() const { return module; }

    private:
        vk::ShaderModule module;

        vk::Device device;
    };

    class SingleUseStagingBuffer
    {
    public:
        SingleUseStagingBuffer() = default;
        ~SingleUseStagingBuffer()
        {
            if (data)
                device.unmapMemory(memory);

            if (memory)
                device.freeMemory(memory);

            if (buffer)
                device.destroyBuffer(buffer);
        }

        [[nodiscard]] bool init(vk::Device dev, PhysicalDeviceInfo const& physicalDeviceInfo, std::size_t size)
        {
            device = dev;

            vk::BufferCreateInfo bufferCreateInfo({}, sizeof(std::byte) * size, vk::BufferUsageFlagBits::eTransferSrc, vk::SharingMode::eExclusive,
                                                  physicalDeviceInfo.graphicsIndex);
            if (checkError(buffer, device.createBuffer(bufferCreateInfo), "", "Failed to create buffer"))
                return false;
            auto memoryRequirements = device.getBufferMemoryRequirements(buffer);

            vk::MemoryPropertyFlags memoryProperties{vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent};

            auto selectedMemoryType = physicalDeviceInfo.getMemoryTypeIndex(memoryRequirements, memoryProperties);
            if (selectedMemoryType == -1)
                return false;

            vk::MemoryAllocateFlagsInfo memoryAllocateFlagsInfo;
            vk::MemoryAllocateInfo memoryAllocateInfo(memoryRequirements.size, selectedMemoryType, &memoryAllocateFlagsInfo);
            if (checkError(memory, device.allocateMemory(memoryAllocateInfo), "", "Failed to allocate buffer memory"))
                return false;

            if (checkError(device.bindBufferMemory(buffer, memory, 0), "", "Failed to bind buffer memory"))
                return false;

            if (checkError(data, device.mapMemory(memory, 0, bufferCreateInfo.size, {}), "", "Failed to map buffer memory"))
                return false;

            return true;
        }

        [[nodiscard]] auto getBuffer() const { return buffer; }
        [[nodiscard]] auto getMemory() const { return memory; }
        [[nodiscard]] auto getData() const { return data; }

    private:
        vk::Buffer buffer;
        vk::DeviceMemory memory;
        void* data{};

        vk::Device device;
    };

    // Class for creating and submitting one time command buffers
    class SingleUseCommandBuffer
    {
    public:
        SingleUseCommandBuffer() = default;
        ~SingleUseCommandBuffer()
        {
            if (commandBuffer)
                device.freeCommandBuffers(commandPool, 1, &commandBuffer);
        }

        // Create command buffer resources
        [[nodiscard]] bool init(vk::Device dev, vk::CommandPool pool)
        {
            device = dev;
            commandPool = pool;

            vk::CommandBufferAllocateInfo allocateInfo(commandPool, vk::CommandBufferLevel::ePrimary, 1);
            if (checkError(device.allocateCommandBuffers(&allocateInfo, &commandBuffer), "", "Failed to allocate single use command buffer"))
                return false;

            vk::CommandBufferBeginInfo beginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
            if (checkError(commandBuffer.begin(beginInfo), "", "Failed to begin command buffer"))
                return false;

            return true;
        }

        // Submit command buffer
        [[nodiscard]] bool submit(vk::Queue submitQueue)
        {
            if (checkError(commandBuffer.end(), "", "Failed to end command buffer"))
                return false;

            vk::SubmitInfo submitInfo({}, {}, commandBuffer);
            if (checkError(submitQueue.submit(submitInfo, {}), "", "Failed to submit command buffer"))
                return false;

            if (checkError(submitQueue.waitIdle(), "", "Failed to wait after submitting command buffer"))
                return false;

            return true;
        }

        [[nodiscard]] auto getCommandBuffer() const { return commandBuffer; }

    private:
        vk::CommandBuffer commandBuffer;

        vk::Device device;
        vk::CommandPool commandPool;
    };

    // Struct for shader constants
    struct PushConstantsBlock
    {
        vk::DeviceAddress quadReference{};
    };

public:
    RenderEngine() = default;

    // Initialize Vulkan resources
    [[nodiscard]] bool init(std::string_view appName, std::uint32_t majorVersion, std::uint32_t minorVersion, std::uint32_t patchVersion)
    {
        VULKAN_HPP_DEFAULT_DISPATCHER.init();

        // Get available instance extensions
        Array<vk::ExtensionProperties, 256> availableInstanceExtensionProperties;
        std::uint32_t instanceExtensionCount = 256;
        if (checkError(vk::enumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, availableInstanceExtensionProperties.getData()), ""sv,
                       "Failed to enumerate available instance extensions"sv))
            return false;

        // Print available instance extensions
        logger.logInfo("{} instance extensions available:", instanceExtensionCount);
        for (std::uint32_t i{}; i < instanceExtensionCount; i++)
            logger.logInfo("\t{}", availableInstanceExtensionProperties[i].extensionName.data());

        // Get available instance layers
        Array<vk::LayerProperties, 256> availableInstanceLayerProperties;
        std::uint32_t instanceLayerCount = 256;
        if (checkError(vk::enumerateInstanceLayerProperties(&instanceLayerCount, availableInstanceLayerProperties.getData()), "",
                       "Failed to enumerate available validation layers"))
            return false;

        // Print available instance layers
        logger.logInfo("{} validation layers available:", instanceLayerCount);
        for (std::uint32_t i{}; i < instanceLayerCount; i++)
            logger.logInfo("\t{}", availableInstanceLayerProperties[i].layerName.data());

        // Define application info
        auto applicationVersion{vk::makeVersion(majorVersion, minorVersion, patchVersion)};
        vk::ApplicationInfo applicationInfo{appName.data(), applicationVersion, "Quad Engine", vk::makeVersion(0, 1, 0),
                                            vk::makeApiVersion(0, 1, 4, 0)};

        // Define required instance layers
        FixedVector<char const*, 64> requiredLayers;
        if constexpr (isDebugBuild)
            requiredLayers.emplaceBack("VK_LAYER_KHRONOS_validation");
        logger.logInfo("{} Vulkan validation layers required:", requiredLayers.getSize());
        for (auto layer : requiredLayers)
            logger.logInfo("\t{}", layer);

        // Check support for required instance layers
        for (auto layer : requiredLayers)
        {
            bool isSupported{};
            for (std::uint32_t i{}; i < instanceLayerCount; i++)
            {
                if (std::strcmp(layer, availableInstanceLayerProperties[i].layerName.data()) != 0)
                    continue;

                isSupported = true;
                break;
            }

            if (!isSupported)
            {
                logger.logError("Instance layer {} is not supported", layer);
                return false;
            }
        }

        // Define required instance extensions
        FixedVector<char const*, 64> requiredInstanceExtensions;
        for (auto extension : renderWindow.getRequiredExtensions())
            requiredInstanceExtensions.emplaceBack(extension);
        if constexpr (isDebugBuild)
            requiredInstanceExtensions.emplaceBack(vk::EXTDebugUtilsExtensionName);

        // Print required instance extensions
        logger.logInfo("{} Vulkan instance extensions required:", requiredInstanceExtensions.getSize());
        for (auto extension : requiredInstanceExtensions)
            logger.logInfo("\t{}", extension);

        // Check support for required instance extensions
        for (auto extension : requiredInstanceExtensions)
        {
            bool isSupported{};
            for (std::size_t i{}; i < instanceExtensionCount; i++)
            {
                if (std::strcmp(extension, availableInstanceExtensionProperties[i].extensionName.data()) != 0)
                    continue;

                isSupported = true;
                break;
            }

            if (!isSupported)
            {
                logger.logError("Instance extension {} is not supported", extension);
                return false;
            }
        }

        // Create Vulkan instance
        vk::DebugUtilsMessengerCreateInfoEXT messengerCreateInfo{
            {},
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
            vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
                vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
            debugCallback};
        vk::InstanceCreateInfo instanceCreateInfo;
        if constexpr (isDebugBuild)
            instanceCreateInfo = vk::InstanceCreateInfo({}, &applicationInfo, requiredLayers.getSize(), requiredLayers.getData(),
                                                        requiredInstanceExtensions.getSize(), requiredInstanceExtensions.getData(), &messengerCreateInfo);
        else
            instanceCreateInfo = vk::InstanceCreateInfo({}, &applicationInfo, requiredLayers.getSize(), requiredLayers.getData(),
                                                        requiredInstanceExtensions.getSize(), requiredInstanceExtensions.getData(), nullptr);
        if (checkError(instance, vk::createInstance(instanceCreateInfo), "Created Vulkan instance", "Failed to create Vulkan instance"))
            return false;


        VULKAN_HPP_DEFAULT_DISPATCHER.init(instance);

        // Create debug messenger
        if constexpr (isDebugBuild)
        {
            if (checkError(debugMessenger, instance.createDebugUtilsMessengerEXT(messengerCreateInfo), "Created debug messenger",
                           "Failed to create debug messenger"))
                return false;
        }

        // Create window surface
        surface = renderWindow.createSurface(instance);
        if (!surface)
            return false;
        logger.logInfo("Created surface");

        // Get available physical devices
        Array<vk::PhysicalDevice, 64> availablePhysicalDevices;
        std::uint32_t availablePhysicalDeviceCount{64};
        if (checkError(instance.enumeratePhysicalDevices(&availablePhysicalDeviceCount, availablePhysicalDevices.getData()), "",
                       "Failed to enumerate physical devices"))
            return false;
        if (availablePhysicalDeviceCount == 0)
        {
            logger.logError("No physical device with Vulkan support found. Try updating drivers");
            return false;
        }

        // Print available physical devices
        logger.logInfo("{} physical devices available:", availablePhysicalDeviceCount);
        for (std::uint32_t i{}; i < availablePhysicalDeviceCount; i++)
            logger.logInfo("\t{}", availablePhysicalDevices[i].getProperties().deviceName.data());

        // Define required physical device extensions
        FixedVector<char const*, 64> requiredPhysicalDeviceExtensions;
        requiredPhysicalDeviceExtensions.emplaceBack(vk::KHRSwapchainExtensionName);

        // Print required physical device extensions
        logger.logInfo("{} physical device extensions required:", requiredPhysicalDeviceExtensions.getSize());
        for (auto const& extension : requiredPhysicalDeviceExtensions)
            logger.logInfo("\t{}", extension);

        // Choose the best physical device
        std::uint32_t maxDeviceScore{};
        for (std::uint32_t i = 0; i < availablePhysicalDeviceCount; i++)
        {
            auto currentPhysicalDevice = availablePhysicalDevices[i];
            std::uint32_t currentScore{};
            PhysicalDeviceInfo currentInfo;
            auto& [name, formatCount, formats, presentModeCount, presentModes, surfaceCapabilities, graphicsIndex, presentationIndex, maxSamplerAnisotropy,
                   memoryProperties] = currentInfo;

            // Get physical device properties
            auto deviceProperties = currentPhysicalDevice.getProperties();
            name = deviceProperties.deviceName.data();
            maxSamplerAnisotropy = deviceProperties.limits.maxSamplerAnisotropy;
            logger.logInfo("Checking {} suitability:", name);

            // Get queue family properties
            Array<vk::QueueFamilyProperties, 16> queueFamilyProperties;
            std::uint32_t queueFamilyPropertiesCount{16};
            currentPhysicalDevice.getQueueFamilyProperties(&queueFamilyPropertiesCount, queueFamilyProperties.getData());
            logger.logInfo("\t{} queue families available", queueFamilyPropertiesCount);

            // Check queue graphics, surface and presentation support
            bool hasGraphicsQueueFamily{};
            bool hasPresentationQueueFamily{};
            for (size_t j{}; j < queueFamilyPropertiesCount; j++)
            {
                if (queueFamilyProperties[j].queueFlags & vk::QueueFlagBits::eGraphics)
                {
                    hasGraphicsQueueFamily = true;
                    graphicsIndex = j;
                    logger.logInfo("\tQueue family with index {} supports graphics", j);
                }

                vk::Bool32 surfaceSupport;
                if (checkError(surfaceSupport, currentPhysicalDevice.getSurfaceSupportKHR(j, surface), "", "Failed to get surface support info"))
                    return false;

                if (surfaceSupport)
                {
                    hasPresentationQueueFamily = true;
                    presentationIndex = j;
                    logger.logInfo("\tQueue family with index {} supports presentation", j);
                }

                if (hasGraphicsQueueFamily && hasPresentationQueueFamily)
                    break;
            }
            if (!hasGraphicsQueueFamily)
            {
                logger.logInfo("\tNo queue family with graphics support found");
                continue;
            }
            if (!hasPresentationQueueFamily)
            {
                logger.logInfo("\tNo queue family with presentation support found");
                continue;
            }

            Array<vk::ExtensionProperties, 1024> deviceExtensionProperties;
            std::uint32_t deviceExtensionPropertiesCount{1024};
            if (checkError(
                    currentPhysicalDevice.enumerateDeviceExtensionProperties(nullptr, &deviceExtensionPropertiesCount, deviceExtensionProperties.getData()), "",
                    "Failed to enumerate physical device extension properties"))
                return false;
            logger.logInfo("\t{} physical device extensions available:", deviceExtensionPropertiesCount);
            for (std::uint32_t j{}; j < deviceExtensionPropertiesCount; j++)
                logger.logInfo("\t\t{}", deviceExtensionProperties[j].extensionName.data());

            bool hasUnsupportedExtension{};
            for (auto extension : requiredPhysicalDeviceExtensions)
            {
                bool isSupported{};
                for (auto const& property : deviceExtensionProperties)
                {
                    if (std::strcmp(property.extensionName, extension) != 0)
                        continue;

                    isSupported = true;
                    break;
                }

                if (!isSupported)
                {
                    logger.logInfo("\tRequired physical device extension {} not supported", extension);
                    hasUnsupportedExtension = true;
                    break;
                }
            }
            if (hasUnsupportedExtension)
                continue;

            formatCount = formats.getSize();
            if (checkError(currentPhysicalDevice.getSurfaceFormatsKHR(surface, &formatCount, formats.getData()), "", "\tFailed to get surface formats"))
                return false;
            logger.logInfo("\t{} physical device surface formats available:", formatCount);
            for (std::uint32_t j{}; j < formatCount; j++)
                logger.logInfo("\t\t{} {}", vk::to_string(formats[j].format), vk::to_string(formats[j].colorSpace));

            presentModeCount = presentModes.getSize();
            if (checkError(currentPhysicalDevice.getSurfacePresentModesKHR(surface, &presentModeCount, presentModes.getData()), "",
                           "\tFailed to get surface present modes"))
                return false;
            logger.logInfo("\t{} physical device present modes available:", presentModeCount);
            for (std::uint32_t j{}; j < presentModeCount; j++)
                logger.logInfo("\t\t{}", vk::to_string(presentModes[j]));

            if (checkError(surfaceCapabilities, currentPhysicalDevice.getSurfaceCapabilitiesKHR(surface), "", "\tFailed to get surface capabilities"))
                return false;

            memoryProperties = currentPhysicalDevice.getMemoryProperties();

            if (formatCount == 0 || presentModeCount == 0)
            {
                logger.logInfo("\tPhysical device doesn't support swapchain");
                continue;
            }

            vk::PhysicalDeviceVulkan12Features features12;
            vk::PhysicalDeviceVulkan11Features features11;
            features11.pNext = &features12;
            vk::PhysicalDeviceFeatures2 features({}, &features11);
            currentPhysicalDevice.getFeatures2(&features);
            if (!features.features.shaderInt64)
            {
                logger.logInfo("\tPhysical device doesn't support 64 bit integers");
                continue;
            }
            if (!features.features.samplerAnisotropy)
            {
                logger.logInfo("\tPhysical device doesn't support anisotropic filtering");
                continue;
            }
            if (!features12.scalarBlockLayout)
            {
                logger.logInfo("\tPhysical device doesn't support scalar block layout");
                continue;
            }
            if (!features12.bufferDeviceAddress)
            {
                logger.logInfo("\tPhysical device doesn't support buffer device address");
                continue;
            }

            if (deviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
                currentScore++;
            logger.logInfo("\tPhysical device is a {}", vk::to_string(deviceProperties.deviceType));

            if (currentScore > maxDeviceScore)
            {
                maxDeviceScore = currentScore;
                physicalDevice = currentPhysicalDevice;
                physicalDeviceInfo = currentInfo;
            }
        }
        if (!physicalDevice)
        {
            logger.logError("No suitable physical devices found. Try updating drivers");
            return false;
        }
        logger.logInfo("Picked {} as a suitable physical device", physicalDeviceInfo.name);

        // Define device queues
        std::array queuePriorities{1.0f};
        FixedVector<vk::DeviceQueueCreateInfo, 16> queueCreateInfos;
        queueCreateInfos.emplaceBack(vk::DeviceQueueCreateFlags{}, physicalDeviceInfo.graphicsIndex, queuePriorities);
        if (physicalDeviceInfo.graphicsIndex != physicalDeviceInfo.presentationIndex)
            queueCreateInfos.emplaceBack(vk::DeviceQueueCreateFlags{}, physicalDeviceInfo.presentationIndex, queuePriorities);

        // Create logical device
        vk::PhysicalDeviceVulkan12Features features12;
        features12.bufferDeviceAddress = vk::True;
        features12.scalarBlockLayout = vk::True;
        vk::PhysicalDeviceVulkan11Features features11;
        features11.pNext = &features12;
        vk::PhysicalDeviceFeatures2 requiredPhysicalDeviceFeatures({}, &features11);
        requiredPhysicalDeviceFeatures.features.shaderInt64 = vk::True;
        requiredPhysicalDeviceFeatures.features.samplerAnisotropy = vk::True;
        vk::DeviceCreateInfo deviceCreateInfo{{},
                                              (std::uint32_t)queueCreateInfos.getSize(),
                                              queueCreateInfos.getData(),
                                              (std::uint32_t)requiredLayers.getSize(),
                                              requiredLayers.getData(),
                                              (std::uint32_t)requiredPhysicalDeviceExtensions.getSize(),
                                              requiredPhysicalDeviceExtensions.getData(),
                                              nullptr,
                                              &requiredPhysicalDeviceFeatures};
        if (checkError(device, physicalDevice.createDevice(deviceCreateInfo), "Created logical device", "Failed to create logical device"))
            return false;

        VULKAN_HPP_DEFAULT_DISPATCHER.init(device);

        // Get queues
        graphicsQueue = device.getQueue(physicalDeviceInfo.graphicsIndex, 0);
        presentationQueue = device.getQueue(physicalDeviceInfo.presentationIndex, 0);

        if (!createSwapchain())
            return false;

        // Create command pool
        vk::CommandPoolCreateInfo poolCreateInfo{vk::CommandPoolCreateFlagBits::eResetCommandBuffer, physicalDeviceInfo.graphicsIndex};
        if (checkError(commandPool, device.createCommandPool(poolCreateInfo), "Created command pool", "Failed to create command pool"))
            return false;

        // Create texture resources
        {
            auto tileImage = ImageLoader("textures/tiles.png"sv);
            vk::DeviceSize imageSize{(std::size_t)tileImage.width * tileImage.height * tileImage.channels};

            SingleUseStagingBuffer stagingBufferResources;
            if (!stagingBufferResources.init(device, physicalDeviceInfo, imageSize))
                return false;
            std::memcpy(stagingBufferResources.getData(), tileImage.data, imageSize);

            vk::ImageCreateInfo imageCreateInfo(
                {}, vk::ImageType::e2D, vk::Format::eR8Unorm, vk::Extent3D{(std::uint32_t)tileImage.width, (std::uint32_t)tileImage.height, 1u}, 1, 1,
                vk::SampleCountFlagBits::e1, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
                vk::SharingMode::eExclusive, physicalDeviceInfo.graphicsIndex, vk::ImageLayout::eUndefined);
            if (checkError(textureImage, device.createImage(imageCreateInfo), "", "Failed to create texture image"))
                return false;

            auto memoryRequirements = device.getImageMemoryRequirements(textureImage);
            vk::MemoryAllocateInfo imageMemoryAllocateInfo(memoryRequirements.size,
                                                           physicalDeviceInfo.getMemoryTypeIndex(memoryRequirements, vk::MemoryPropertyFlagBits::eDeviceLocal));
            if (checkError(textureImageMemory, device.allocateMemory(imageMemoryAllocateInfo), "", "Failed to allocate texture memory"))
                return false;

            if (checkError(device.bindImageMemory(textureImage, textureImageMemory, 0), "", "Failed to bind tile texture memory"))
                return false;

            {
                SingleUseCommandBuffer transitionCommandBuffer;
                if (!transitionCommandBuffer.init(device, commandPool))
                    return false;

                vk::ImageSubresourceRange range(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
                vk::ImageMemoryBarrier memoryBarrier(vk::AccessFlagBits::eNone, vk::AccessFlagBits::eTransferWrite, vk::ImageLayout::eUndefined,
                                                     vk::ImageLayout::eTransferDstOptimal, vk::QueueFamilyIgnored, vk::QueueFamilyIgnored, textureImage, range);
                transitionCommandBuffer.getCommandBuffer().pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eTransfer, {}, {},
                                                                           {}, memoryBarrier);

                if (!transitionCommandBuffer.submit(graphicsQueue))
                    return false;
            }

            {
                SingleUseCommandBuffer copyCommandBuffer;
                if (!copyCommandBuffer.init(device, commandPool))
                    return false;

                vk::ImageSubresourceLayers imageSubresourceLayers(vk::ImageAspectFlagBits::eColor, 0, 0, 1);
                vk::BufferImageCopy imageCopy(0, 0, 0, imageSubresourceLayers, {}, {(uint32_t)tileImage.width, (uint32_t)tileImage.height, 1});
                copyCommandBuffer.getCommandBuffer().copyBufferToImage(stagingBufferResources.getBuffer(), textureImage, vk::ImageLayout::eTransferDstOptimal,
                                                                       imageCopy);

                if (!copyCommandBuffer.submit(graphicsQueue))
                    return false;
            }

            {
                SingleUseCommandBuffer transitionCommandBuffer;
                if (!transitionCommandBuffer.init(device, commandPool))
                    return false;

                vk::ImageSubresourceRange range(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
                vk::ImageMemoryBarrier memoryBarrier(vk::AccessFlagBits::eTransferWrite, vk::AccessFlagBits::eShaderRead, vk::ImageLayout::eTransferDstOptimal,
                                                     vk::ImageLayout::eShaderReadOnlyOptimal, vk::QueueFamilyIgnored, vk::QueueFamilyIgnored, textureImage,
                                                     range);
                transitionCommandBuffer.getCommandBuffer().pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader, {},
                                                                           {}, {}, memoryBarrier);

                if (!transitionCommandBuffer.submit(graphicsQueue))
                    return false;
            }

            vk::ImageSubresourceRange subresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
            vk::ImageViewCreateInfo viewCreateInfo({}, textureImage, vk::ImageViewType::e2D, vk::Format::eR8Unorm, {}, subresourceRange);
            if (checkError(textureImageView, device.createImageView(viewCreateInfo, nullptr), "", "Failed to create texture image view"))
                return false;

            vk::SamplerCreateInfo samplerCreateInfo({}, vk::Filter::eLinear, vk::Filter::eLinear, vk::SamplerMipmapMode::eLinear,
                                                    vk::SamplerAddressMode::eRepeat, vk::SamplerAddressMode::eRepeat, vk::SamplerAddressMode::eRepeat, 0.0f,
                                                    vk::True, physicalDeviceInfo.maxSamplerAnisotropy, vk::False, vk::CompareOp::eAlways, 0.0f, 0.0f,
                                                    vk::BorderColor::eIntOpaqueBlack, vk::False);
            if (checkError(textureSampler, device.createSampler(samplerCreateInfo), "", "Failed to create texture sampler"))
                return false;
        }

        vk::DescriptorSetLayoutBinding layoutBinding(0, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment, {});
        vk::DescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo({}, layoutBinding);
        if (checkError(descriptorSetLayout, device.createDescriptorSetLayout(descriptorSetLayoutCreateInfo), "Created descriptor set layout",
                       "Failed to create descriptor set layout"))
            return false;

        vk::DescriptorPoolSize descriptorPoolSize(vk::DescriptorType::eCombinedImageSampler, maxFramesInFlight);
        vk::DescriptorPoolCreateInfo descriptorPoolCreateInfo({}, maxFramesInFlight, descriptorPoolSize);
        if (checkError(descriptorPool, device.createDescriptorPool(descriptorPoolCreateInfo), "Created descriptor pool", "Failed to create descriptor pool"))
            return false;

        std::array<vk::DescriptorSetLayout, maxFramesInFlight> setLayouts{descriptorSetLayout, descriptorSetLayout};
        vk::DescriptorSetAllocateInfo descriptorSetAllocateInfo(descriptorPool, setLayouts);
        if (checkError(device.allocateDescriptorSets(&descriptorSetAllocateInfo, descriptorSets.getData()), "Allocated descriptor sets",
                       "Failed to allocate descriptor sets"))
            return false;

        for (size_t i = 0; i < maxFramesInFlight; i++)
        {
            vk::DescriptorImageInfo imageInfo(textureSampler, textureImageView, vk::ImageLayout::eShaderReadOnlyOptimal);
            vk::WriteDescriptorSet writeDescriptorSet(descriptorSets[i], 0, 0, vk::DescriptorType::eCombinedImageSampler, imageInfo);
            device.updateDescriptorSets(writeDescriptorSet, {});
        }

        {
            // Create shader modules
            ShaderModule vertexShaderModule;
            if (!vertexShaderModule.init(device, "shaders/quadVert.spv"sv))
                return false;
            ShaderModule fragmentShaderModule;
            if (!fragmentShaderModule.init(device, "shaders/quadFrag.spv"sv))
                return false;

            // Define shader stages
            Array stageCreateInfos{vk::PipelineShaderStageCreateInfo{{}, vk::ShaderStageFlagBits::eVertex, vertexShaderModule.getModule(), "main"},
                                   vk::PipelineShaderStageCreateInfo{{}, vk::ShaderStageFlagBits::eFragment, fragmentShaderModule.getModule(), "main"}};

            // Define dynamic states
            Array dynamicStates{vk::DynamicState::eViewport, vk::DynamicState::eScissor};
            vk::PipelineDynamicStateCreateInfo dynamicStateCreateInfo{{}, dynamicStates.getSize(), dynamicStates.getData()};

            // Define vertex input
            vk::PipelineVertexInputStateCreateInfo vertexInputStateCreateInfo{{}, nullptr, nullptr};

            // Define input assembly
            vk::PipelineInputAssemblyStateCreateInfo assemblyStateCreateInfo{{}, vk::PrimitiveTopology::eTriangleStrip, vk::False};

            // Define viewport
            vk::Viewport viewport{0.0f, 0.0f, (float)swapchainResources.imageExtent.width, (float)swapchainResources.imageExtent.height, 0.0f, 1.0f};
            vk::Rect2D scissor{{0, 0}, swapchainResources.imageExtent};
            vk::PipelineViewportStateCreateInfo viewportStateCreateInfo{{}, viewport, scissor};

            // Define rasterization
            vk::PipelineRasterizationStateCreateInfo rasterizationStateCreateInfo{
                {}, vk::False, vk::False, vk::PolygonMode::eFill, vk::CullModeFlagBits::eBack, vk::FrontFace::eClockwise, vk::False, 0.0f, 0.0f, 0.0f, 1.0f};

            // Define multisampling
            vk::PipelineMultisampleStateCreateInfo multisampleStateCreateInfo{{}, vk::SampleCountFlagBits::e1, vk::False, 0.0, nullptr, vk::False, vk::False};

            // Define depth and stencil
            vk::PipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo{{}, vk::False, vk::False, vk::CompareOp::eNever, vk::False, vk::False};

            // Define color blending
            vk::PipelineColorBlendAttachmentState colorBlendAttachmentState{vk::True,
                                                                            vk::BlendFactor::eSrcAlpha,
                                                                            vk::BlendFactor::eOneMinusSrcAlpha,
                                                                            vk::BlendOp::eAdd,
                                                                            vk::BlendFactor::eOne,
                                                                            vk::BlendFactor::eZero,
                                                                            vk::BlendOp::eAdd,
                                                                            vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                                                                                vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA};
            vk::PipelineColorBlendStateCreateInfo colorBlendStateCreateInfo{
                {}, vk::False, vk::LogicOp::eNoOp, colorBlendAttachmentState, {1.0f, 1.0f, 1.0f, 1.0f}};

            // Create pipeline layout
            vk::PushConstantRange pushConstantRange(vk::ShaderStageFlagBits::eVertex, 0, sizeof(QuadData));
            vk::PipelineLayoutCreateInfo layoutCreateInfo({}, descriptorSetLayout, pushConstantRange);
            if (checkError(pipelineLayout, device.createPipelineLayout(layoutCreateInfo), "Created pipeline layout", "Failed to create pipeline layout"))
                return false;

            // Create graphics pipeline
            vk::GraphicsPipelineCreateInfo pipelineCreateInfo({}, stageCreateInfos.getSize(), stageCreateInfos.getData(), &vertexInputStateCreateInfo,
                                                              &assemblyStateCreateInfo, nullptr, &viewportStateCreateInfo, &rasterizationStateCreateInfo,
                                                              &multisampleStateCreateInfo, &depthStencilStateCreateInfo, &colorBlendStateCreateInfo,
                                                              &dynamicStateCreateInfo, pipelineLayout, swapchainResources.renderPass, 0);
            if (checkError(graphicsPipeline, device.createGraphicsPipeline({}, pipelineCreateInfo), "Created graphics pipeline",
                           "Failed to create graphics pipeline"))
                return false;
        }

        for (std::uint64_t i{}; i < quadDataBuffers.getSize(); i++)
        {
            auto& bufferResources = quadDataBuffers[i];

            vk::BufferCreateInfo bufferCreateInfo({}, sizeof(QuadData) * QuadPool::getCapacity(), vk::BufferUsageFlagBits::eShaderDeviceAddress,
                                                  vk::SharingMode::eExclusive, physicalDeviceInfo.graphicsIndex);
            if (checkError(bufferResources.buffer, device.createBuffer(bufferCreateInfo), "", "Failed to create buffer"))
                return false;
            auto bufferMemoryRequirements = device.getBufferMemoryRequirements(bufferResources.buffer);

            vk::MemoryPropertyFlags memoryProperties{vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent};
            memoryProperties = {vk::MemoryPropertyFlagBits::eDeviceLocal | vk::MemoryPropertyFlagBits::eHostVisible |
                                vk::MemoryPropertyFlagBits::eHostCoherent};

            auto selectedMemoryType = physicalDeviceInfo.getMemoryTypeIndex(bufferMemoryRequirements, memoryProperties);
            if (selectedMemoryType == -1)
                return false;

            vk::MemoryAllocateFlagsInfo memoryAllocateFlagsInfo{vk::MemoryAllocateFlagBits::eDeviceAddress};
            vk::MemoryAllocateInfo memoryAllocateInfo(bufferMemoryRequirements.size, selectedMemoryType, &memoryAllocateFlagsInfo);
            if (checkError(bufferResources.bufferMemory, device.allocateMemory(memoryAllocateInfo), "", "Failed to allocate buffer memory"))
                return false;

            if (checkError(device.bindBufferMemory(bufferResources.buffer, bufferResources.bufferMemory, 0), "", "Failed to bind buffer memory"))
                return false;

            vk::BufferDeviceAddressInfo deviceAddressInfo(bufferResources.buffer);
            bufferResources.bufferAddress = device.getBufferAddress(deviceAddressInfo);
            if (!bufferResources.bufferAddress)
            {
                logger.logError("Failed to get buffer address");
                return false;
            }

            if (checkError(bufferResources.data, device.mapMemory(bufferResources.bufferMemory, 0, bufferCreateInfo.size, {}), "",
                           "Failed to map buffer memory"))
                return false;
        }
        logger.logInfo("Created quad data buffers");

        // Allocate command buffers
        vk::CommandBufferAllocateInfo bufferAllocateInfo{commandPool, vk::CommandBufferLevel::ePrimary, maxFramesInFlight};
        if (checkError(device.allocateCommandBuffers(&bufferAllocateInfo, commandBuffers.getData()), "Allocated command buffer",
                       "Failed to allocate command buffer"))
            return false;

        // Create synchronization objects
        vk::FenceCreateInfo fenceCreateInfo{vk::FenceCreateFlagBits::eSignaled};
        for (std::size_t i{}; i < maxFramesInFlight; i++)
        {
            if (checkError(imageAcquiredSemaphores[i], device.createSemaphore({}), "", "Failed to create semaphore") ||
                checkError(inFlightFences[i], device.createFence(fenceCreateInfo), "", "Failed to create fence"))
                return false;
        }

        submitFinishedSemaphores.resize(swapchainResources.imageCount);
        for (std::size_t i{}; i < swapchainResources.imageCount; i++)
        {
            if (checkError(submitFinishedSemaphores[i], device.createSemaphore({}), "", "Failed to create semaphore"))
                return false;
        }
        logger.logInfo("Created synchronization objects");

        return true;
    }
    void cleanup()
    {
        if (device)
        {
            auto _ = device.waitIdle();

            for (auto fence : inFlightFences)
                device.destroyFence(fence);

            for (auto semaphore : submitFinishedSemaphores)
                device.destroySemaphore(semaphore);

            for (auto semaphore : imageAcquiredSemaphores)
                device.destroySemaphore(semaphore);

            device.freeCommandBuffers(commandPool, commandBuffers.getSize(), commandBuffers.getData());

            for (auto buffer : quadDataBuffers)
            {
                device.freeMemory(buffer.bufferMemory);
                device.destroyBuffer(buffer.buffer);
            }

            device.destroyPipeline(graphicsPipeline);
            device.destroyPipelineLayout(pipelineLayout);

            device.destroyDescriptorPool(descriptorPool);
            device.destroyDescriptorSetLayout(descriptorSetLayout);

            device.destroySampler(textureSampler);
            device.destroyImageView(textureImageView);
            device.freeMemory(textureImageMemory);
            device.destroyImage(textureImage);

            device.destroyCommandPool(commandPool);

            if (swapchainResources.swapchain)
            {
                device.destroyRenderPass(swapchainResources.renderPass);

                for (std::uint32_t i{}; i < swapchainResources.imageCount; i++)
                {
                    device.destroyFramebuffer(swapchainResources.framebuffers[i]);
                    device.destroyImageView(swapchainResources.imageViews[i]);
                }

                device.destroySwapchainKHR(swapchainResources.swapchain);
            }

            device.destroy();
        }

        if (surface)
            instance.destroy(surface);

        if (debugMessenger)
            instance.destroyDebugUtilsMessengerEXT(debugMessenger);

        if (instance)
            instance.destroy();
    }

    // Submit drawing commands for current frame
    [[nodiscard]] bool drawFrame()
    {
        auto timeout = std::numeric_limits<std::uint64_t>::max();
        if (checkError(device.waitForFences(inFlightFences[currentFrameIndex], vk::True, timeout), "", "Failed to wait for fence"))
            return false;
        if (checkError(device.resetFences(inFlightFences[currentFrameIndex]), "", "Failed to reset fence"))
            return false;

        auto [result, imageIndex] = device.acquireNextImageKHR(swapchainResources.swapchain, timeout, imageAcquiredSemaphores[currentFrameIndex], {});
        if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR)
            return recreateSwapchain();

        if (checkError(result, "", "Failed to acquire next image"))
            return false;

        auto commandBuffer = commandBuffers[currentFrameIndex];

        if (checkError(commandBuffer.reset({}), "", "Failed to reset command buffer"))
            return false;

        auto instanceCount = copyQuadPools();

        vk::CommandBufferBeginInfo beginInfo;
        if (checkError(commandBuffer.begin(beginInfo), "", "Failed to begin command buffer"))
            return false;

        vk::Rect2D renderArea({0, 0}, swapchainResources.imageExtent);
        vk::ClearValue clearValue(vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f));
        vk::RenderPassBeginInfo renderPassBeginInfo(swapchainResources.renderPass, swapchainResources.framebuffers[imageIndex], renderArea, clearValue);
        commandBuffer.beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);

        commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, graphicsPipeline);

        vk::Viewport viewport(0.0f, 0.0f, swapchainResources.imageExtent.width, swapchainResources.imageExtent.height, 0.0f, 1.0f);
        commandBuffer.setViewport(0, viewport);

        vk::Rect2D scissor({0, 0}, swapchainResources.imageExtent);
        commandBuffer.setScissor(0, scissor);

        PushConstantsBlock pushConstants{quadDataBuffers[currentFrameIndex].bufferAddress};
        commandBuffer.pushConstants<PushConstantsBlock>(pipelineLayout, vk::ShaderStageFlagBits::eVertex, 0u, pushConstants);

        commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLayout, 0, descriptorSets[currentFrameIndex], {});
        commandBuffer.draw(4, instanceCount, 0, 0);

        commandBuffer.endRenderPass();

        if (checkError(commandBuffer.end(), "", "Failed to end command buffer"))
            return false;

        vk::PipelineStageFlags waitStage(vk::PipelineStageFlagBits::eColorAttachmentOutput);
        vk::SubmitInfo submitInfo(imageAcquiredSemaphores[currentFrameIndex], waitStage, commandBuffers[currentFrameIndex],
                                  submitFinishedSemaphores[imageIndex]);
        if (checkError(graphicsQueue.submit(submitInfo, inFlightFences[currentFrameIndex]), "", "Failed to submit to graphics queue"))
            return false;

        vk::PresentInfoKHR presentInfo(submitFinishedSemaphores[imageIndex], swapchainResources.swapchain, imageIndex);
        result = presentationQueue.presentKHR(presentInfo);
        if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR)
            return recreateSwapchain();

        if (checkError(result, "", "Failed to present image"))
            return false;

        currentFrameIndex = (currentFrameIndex + 1) % maxFramesInFlight;

        if (oldSwapchainResources.swapchain)
        {
            if (oldRendersRemaining == 0)
                oldSwapchainResources = {};
            else
                oldRendersRemaining--;
        }

        return true;
    }

private:
    template <class Value, class ResultValue>
    [[nodiscard]] static bool checkError(Value& value, ResultValue resultValue, std::string_view successMessage, std::string_view errorMessage)
    {
        if (checkError(resultValue.result, successMessage, errorMessage))
            return true;

        value = std::move(resultValue.value);
        return false;
    }
    [[nodiscard]] static bool checkError(vk::Result result, std::string_view successMessage, std::string_view errorMessage)
    {
        if (result != vk::Result::eSuccess)
        {
            logger.logError("{}: {}", errorMessage, vk::to_string(result));
            return true;
        }

        if (!successMessage.empty())
            logger.logInfo("{}", successMessage);

        return false;
    }

    template <std::size_t currentIndex = 0>
    std::size_t copyQuadPools(std::size_t instanceCount = 0)
    {
        if constexpr (currentIndex < (std::size_t)QuadLayer::COUNT)
        {
            auto poolData = quadPool.getData<(QuadLayer)currentIndex>();
            std::memcpy(static_cast<char*>(quadDataBuffers[currentFrameIndex].data) + instanceCount * sizeof(QuadData), poolData.data(),
                        poolData.size() * sizeof(QuadData));
            return copyQuadPools<currentIndex + 1>(instanceCount + poolData.size());
        }
        else
        {
            return instanceCount;
        }
    }

    [[nodiscard]] bool createSwapchain()
    {
        auto& res = swapchainResources;

        // Choose surface format
        vk::SurfaceFormatKHR selectedFormat{physicalDeviceInfo.surfaceFormats[0]};
        for (auto format : physicalDeviceInfo.surfaceFormats)
        {
            if (format.format == vk::Format::eB8G8R8A8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
                selectedFormat = format;
        }
        res.imageFormat = selectedFormat.format;
        logger.logInfo("Chose format {} with color space {}", vk::to_string(selectedFormat.format), vk::to_string(selectedFormat.colorSpace));

        // Choose present mode
        auto selectedPresentMode{vk::PresentModeKHR::eFifo};
        for (auto presentMode : physicalDeviceInfo.presentModes)
        {
            if (presentMode == vk::PresentModeKHR::eMailbox)
                selectedPresentMode = presentMode;
        }
        logger.logInfo("Chose present mode {}", vk::to_string(selectedPresentMode));

        // Choose swapchain extent
        auto const& surfaceCapabilities = physicalDeviceInfo.surfaceCapabilities;
        res.imageExtent = surfaceCapabilities.currentExtent;
        if (res.imageExtent.width == std::numeric_limits<std::uint32_t>::max())
        {
            auto framebufferSize = renderWindow.getWindowSize();
            res.imageExtent.width =
                std::clamp((std::uint32_t)framebufferSize.first, surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width);
            res.imageExtent.height =
                std::clamp((std::uint32_t)framebufferSize.second, surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height);
        }
        logger.logInfo("Swap extent is [{},{}]", res.imageExtent.width, res.imageExtent.height);

        // Create swapchain
        vk::SharingMode sharingMode{physicalDeviceInfo.graphicsIndex != physicalDeviceInfo.presentationIndex ? vk::SharingMode::eConcurrent
                                                                                                             : vk::SharingMode::eExclusive};
        FixedVector<std::uint32_t, 2> queueFamilyIndices;
        if (sharingMode == vk::SharingMode::eConcurrent)
        {
            queueFamilyIndices.emplaceBack(physicalDeviceInfo.graphicsIndex);
            queueFamilyIndices.emplaceBack(physicalDeviceInfo.presentationIndex);
        }

        vk::SwapchainCreateInfoKHR swapchainCreateInfo{{},
                                                       surface,
                                                       std::max(surfaceCapabilities.minImageCount, maxFramesInFlight),
                                                       selectedFormat.format,
                                                       selectedFormat.colorSpace,
                                                       res.imageExtent,
                                                       1,
                                                       vk::ImageUsageFlagBits::eColorAttachment,
                                                       sharingMode,
                                                       (std::uint32_t)queueFamilyIndices.getSize(),
                                                       queueFamilyIndices.getData(),
                                                       surfaceCapabilities.currentTransform,
                                                       vk::CompositeAlphaFlagBitsKHR::eOpaque,
                                                       selectedPresentMode,
                                                       vk::True,
                                                       oldSwapchainResources.swapchain};
        if (checkError(res.swapchain, device.createSwapchainKHR(swapchainCreateInfo), "Created swapchain", "Failed to create swapchain"))
            return false;

        // Get swapchain images
        res.imageCount = res.images.getSize();
        if (checkError(device.getSwapchainImagesKHR(res.swapchain, &res.imageCount, res.images.getData()), "", "Failed to get swapchain images"))
            return false;
        logger.logInfo("Image count is {}", res.imageCount);

        // Create swapchain image views
        for (size_t i = 0; i < res.imageCount; i++)
        {
            vk::ImageSubresourceRange subresourceRange{vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1};
            vk::ImageViewCreateInfo viewCreateInfo{
                {},
                res.images[i],
                vk::ImageViewType::e2D,
                res.imageFormat,
                {vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity},
                subresourceRange};
            if (checkError(res.imageViews[i], device.createImageView(viewCreateInfo), "", "Failed to create image view"))
                return false;
        }

        // Define attachment
        vk::AttachmentDescription colorAttachment{{},
                                                  res.imageFormat,
                                                  vk::SampleCountFlagBits::e1,
                                                  vk::AttachmentLoadOp::eClear,
                                                  vk::AttachmentStoreOp::eStore,
                                                  vk::AttachmentLoadOp::eDontCare,
                                                  vk::AttachmentStoreOp::eDontCare,
                                                  vk::ImageLayout::eUndefined,
                                                  vk::ImageLayout::ePresentSrcKHR};
        vk::AttachmentReference colorAttachmentReference{0, vk::ImageLayout::eColorAttachmentOptimal};

        // Create render pass
        vk::SubpassDescription subpassDescription{{}, vk::PipelineBindPoint::eGraphics, {}, colorAttachmentReference};
        vk::SubpassDependency subpassDependency{vk::SubpassExternal,
                                                0,
                                                vk::PipelineStageFlagBits::eColorAttachmentOutput,
                                                vk::PipelineStageFlagBits::eColorAttachmentOutput,
                                                {},
                                                vk::AccessFlagBits::eColorAttachmentWrite};
        vk::RenderPassCreateInfo renderPassCreateInfo{{}, colorAttachment, subpassDescription, subpassDependency};
        if (checkError(res.renderPass, device.createRenderPass(renderPassCreateInfo), "Created render pass", "Failed to create render pass"))
            return false;

        // Create swapchain framebuffers
        for (size_t i = 0; i < res.imageCount; i++)
        {
            vk::FramebufferCreateInfo framebufferCreateInfo{{}, res.renderPass, res.imageViews[i], res.imageExtent.width, res.imageExtent.height, 1};
            if (checkError(res.framebuffers[i], device.createFramebuffer(framebufferCreateInfo), "", "Failed to create swapchain buffer"))
                return false;
        }
        logger.logInfo("Created swapchain framebuffers");

        return true;
    }
    // Create new swapchain and mark old one for deletion
    [[nodiscard]] bool recreateSwapchain()
    {
        if (checkError(physicalDeviceInfo.surfaceCapabilities, physicalDevice.getSurfaceCapabilitiesKHR(surface), "", "Failed to get surface capabilities"))
            return false;

        if (physicalDeviceInfo.surfaceCapabilities.currentExtent.width == 0 || physicalDeviceInfo.surfaceCapabilities.currentExtent.height == 0)
            return true;

        if (checkError(physicalDevice.getSurfaceFormatsKHR(surface, &physicalDeviceInfo.surfaceFormatCount, physicalDeviceInfo.surfaceFormats.getData()), "",
                       "Failed to get surface formats"))
            return false;

        if (checkError(physicalDevice.getSurfacePresentModesKHR(surface, &physicalDeviceInfo.presentModeCount, physicalDeviceInfo.presentModes.getData()), "",
                       "Failed to get surface presentation modes"
                       "Failed to get surface present modes"))
            return false;

        oldSwapchainResources = swapchainResources;
        oldRendersRemaining = oldSwapchainResources.imageCount;

        return createSwapchain();
    }

    // Callback for debug utils messenger
    static VKAPI_ATTR vk::Bool32 VKAPI_CALL debugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                          vk::DebugUtilsMessageTypeFlagsEXT messageType,
                                                          vk::DebugUtilsMessengerCallbackDataEXT const* pCallbackData, void* pUserData)
    {
        if (messageSeverity <= vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning)
            logger.logInfo("{}", pCallbackData->pMessage);
        else if (messageSeverity == vk::DebugUtilsMessageSeverityFlagBitsEXT::eError)
            logger.logError("{}", pCallbackData->pMessage);

        return vk::False;
    }

    static constexpr std::uint32_t maxFramesInFlight{2};

    vk::Instance instance;
    vk::DebugUtilsMessengerEXT debugMessenger;
    vk::SurfaceKHR surface;
    vk::PhysicalDevice physicalDevice;
    PhysicalDeviceInfo physicalDeviceInfo;
    vk::Device device;
    vk::Queue graphicsQueue;
    vk::Queue presentationQueue;
    SwapchainResources swapchainResources;
    vk::CommandPool commandPool;
    vk::Image textureImage;
    vk::DeviceMemory textureImageMemory;
    vk::ImageView textureImageView;
    vk::Sampler textureSampler;
    vk::DescriptorSetLayout descriptorSetLayout;
    vk::DescriptorPool descriptorPool;
    Array<vk::DescriptorSet, maxFramesInFlight> descriptorSets;
    vk::PipelineLayout pipelineLayout;
    vk::Pipeline graphicsPipeline;
    Array<BufferResources, maxFramesInFlight> quadDataBuffers;
    Array<vk::CommandBuffer, maxFramesInFlight> commandBuffers;

    Array<vk::Semaphore, maxFramesInFlight> imageAcquiredSemaphores;
    FixedVector<vk::Semaphore, 8> submitFinishedSemaphores;
    Array<vk::Fence, maxFramesInFlight> inFlightFences;
    std::uint32_t currentFrameIndex{};

    std::uint32_t oldRendersRemaining{};
    SwapchainResources oldSwapchainResources;
};
export inline RenderEngine renderEngine;

/*
 * Implementation of QuadReference methods
 */
template <QuadLayer layer>
QuadReference<layer>::~QuadReference()
{
    if (index != -1)
        clear();
}
template <QuadLayer layer>
QuadReference<layer>& QuadReference<layer>::operator=(QuadReference&& rhs) noexcept
{
    auto& references = quadPool.getStorage<layer>().references;

    //Update reference pointers in quad pool
    if (index != -1)
        references[index] = &rhs;
    if (rhs.index != -1)
        references[rhs.index] = this;

    std::swap(index, rhs.index);
    return *this;
}
template <QuadLayer layer>
void QuadReference<layer>::setPosition(float positionX, float positionY) const
{
    logger.extraAssert(index != -1, "Set position of invalid quad reference");

    auto& data = quadPool.getStorage<layer>().data;
    data[index].setPosition(positionX, positionY);
}
template <QuadLayer layer>
void QuadReference<layer>::setGlyph(std::uint8_t glyph) const
{
    logger.extraAssert(index != -1, "Set glyph of invalid quad reference");

    auto& data = quadPool.getStorage<layer>().data;
    data[index].setGlyph(glyph);
}
template <QuadLayer layer>
void QuadReference<layer>::setColor(std::uint32_t packedColor) const
{
    logger.extraAssert(index != -1, "Set color of invalid quad reference");

    auto& data = quadPool.getStorage<layer>().data;
    data[index].setColor(packedColor);
}
template <QuadLayer layer>
void QuadReference<layer>::setBackgroundColor(std::uint32_t packedColor) const
{
    logger.extraAssert(index != -1, "Set background color of invalid quad reference");

    auto& data = quadPool.getStorage<layer>().data;
    data[index].setBackgroundColor(packedColor);
}
template <QuadLayer layer>
template <IsSameType<QuadData> T>
void QuadReference<layer>::init(T&& quadData)
{
    logger.extraAssert(index == -1, "QuadReference::init() - initialized already existing reference");

    auto& [data, references] = quadPool.getStorage<layer>();

    logger.extraAssert(data.getSize() < data.getCapacity(), "QuadReference::init() - quad pool was full");

    //Append reference to the end of the quad pool
    index = (std::int64_t)data.getSize();
    data.emplaceBack(std::forward<T>(quadData));
    references.emplaceBack(this);
}
template <QuadLayer layer>
void QuadReference<layer>::clear()
{
    logger.extraAssert(index != -1, "Clearing invalid QuadReference");

    auto& [data, references] = quadPool.getStorage<layer>();

    //Copy last element in place of deleted and pop
    data[index] = data.getBack();
    data.popBack();

    //Update the reference index
    references[index] = references.getBack();
    references[index]->index = index;
    references.popBack();

    index = -1;
}