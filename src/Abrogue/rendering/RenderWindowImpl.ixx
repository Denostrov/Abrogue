module Abrogue:RenderWindowImpl;

import :RenderWindow;
import :Configuration;

using namespace std::literals;

/*
 * Implementation of RenderWindow methods
 */
bool RenderWindow::init()
{
    //Set application metadata
    auto fullAppName = Constants::appName.data() + " "s + Constants::appVersion.data();
    if (checkError(SDL_SetAppMetadataProperty(sdlPropAppMetadataNameString, fullAppName.c_str()) &&
        SDL_SetAppMetadataProperty(sdlPropAppMetadataVersionString, Constants::appVersion.data()) &&
        SDL_SetAppMetadataProperty(sdlPropAppMetadataIdentifierString, Constants::appIdentifier.data()) &&
        SDL_SetAppMetadataProperty(sdlPropAppMetadataCreatorString, Constants::appCreator.data()) &&
        SDL_SetAppMetadataProperty(sdlPropAppMetadataCopyrightString, Constants::appCopyright.data()) &&
        SDL_SetAppMetadataProperty(sdlPropAppMetadataTypeString, Constants::appType.data())))
        return false;

    //Initialize SDL video system
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

    auto windowWidth = std::min((std::int64_t)displayUsableRect.w, configuration.getWindowWidth());
    auto windowHeight = std::min((std::int64_t)displayUsableRect.h, configuration.getWindowHeight());

    //Create window
    window = SDL_CreateWindow(fullAppName.c_str(), windowWidth, windowHeight,
                              sdlWindowVulkan | sdlWindowResizable | (configuration.getIsFullscreen()
                                                                          ? sdlWindowFullscreen
                                                                          : configuration.getIsMaximized()
                                                                          ? sdlWindowMaximized
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

    //Get required extensions
    std::uint32_t extensionCount{};
    auto vulkanExtensions = SDL_Vulkan_GetInstanceExtensions(&extensionCount);
    if (checkError(vulkanExtensions))
        return false;

    requiredExtensions = {vulkanExtensions, extensionCount};

    return true;
}
RenderWindow::~RenderWindow()
{
    if (window)
        SDL_DestroyWindow(window);

    if (SDL_WasInit(sdlInitVideo))
        SDL_QuitSubSystem(sdlInitVideo);

    SDL_Quit();
}
VkSurfaceKHR RenderWindow::createSurface(VkInstance instance) const
{
    VkSurfaceKHR result;
    if (checkError(SDL_Vulkan_CreateSurface(window, instance, nullptr, &result)))
        return VkSurfaceKHR{};

    return result;
}
std::pair<std::int64_t, std::int64_t> RenderWindow::getWindowSize() const
{
    int width{}, height{};
    SDL_GetWindowSizeInPixels(window, &width, &height);
    return {width, height};
}
void RenderWindow::setIsMaximized(bool maximized) const
{
    maximized ? SDL_MaximizeWindow(window) : SDL_RestoreWindow(window);
    SDL_SyncWindow(window);
}
void RenderWindow::setIsFullscreen(bool fullscreen) const
{
    SDL_SetWindowFullscreen(window, fullscreen);
    SDL_SyncWindow(window);
}
bool RenderWindow::checkError(bool value)
{
    if (value)
        return false;

    if (auto errorString = SDL_GetError(); errorString[0] != '\0')
    {
        logger.logError(errorString);
        SDL_ClearError();
    }

    return true;
}