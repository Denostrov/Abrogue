module;

#include <SDL3/SDL_messagebox.h>
#include <SDL3/SDL_scancode.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_vulkan.h>

#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>

export module SDL3;

export
{
    using ::SDL_AppInit;
    using ::SDL_AppIterate;
    using ::SDL_AppEvent;
    using ::SDL_AppQuit;

    using ::SDL_Init;
    using ::SDL_GetError;
    using ::SDL_ClearError;
    using ::SDL_GetDisplays;
    using ::SDL_GetDisplayUsableBounds;
    using ::SDL_GetMouseState;
    using ::SDL_CreateWindow;
    using ::SDL_SetWindowPosition;
    using ::SDL_ShowSimpleMessageBox;
    using ::SDL_GetWindowFlags;
    using ::SDL_GetWindowSizeInPixels;
    using ::SDL_GetTicksNS;
    using ::SDL_MaximizeWindow;
    using ::SDL_RestoreWindow;
    using ::SDL_SyncWindow;
    using ::SDL_SetWindowFullscreen;
    using ::SDL_SetAppMetadataProperty;
    using ::SDL_DestroyWindow;
    using ::SDL_WasInit;
    using ::SDL_QuitSubSystem;
    using ::SDL_Quit;
    using ::SDL_free;
    using ::SDL_Vulkan_GetInstanceExtensions;
    using ::SDL_Vulkan_CreateSurface;
    using ::SDL_Vulkan_DestroySurface;

    using ::SDL_Scancode;
    using enum ::SDL_Scancode;

    using ::SDL_AppResult;
    using enum ::SDL_AppResult;

    using ::SDL_Event;
    using ::SDL_EventType;
    using enum ::SDL_EventType;

    using ::SDL_Window;
    using ::SDL_Rect;

    using ::VkSurfaceKHR;
    using ::VkInstance;

    constexpr auto sdlInitVideo = SDL_INIT_VIDEO;
    constexpr auto sdlMessageboxError = SDL_MESSAGEBOX_ERROR;

    constexpr auto sdlWindowMaximized = SDL_WINDOW_MAXIMIZED;
    constexpr auto sdlWindowFullscreen = SDL_WINDOW_FULLSCREEN;
    constexpr auto sdlWindowVulkan = SDL_WINDOW_VULKAN;
    constexpr auto sdlWindowResizable = SDL_WINDOW_RESIZABLE;

    constexpr auto sdlPropAppMetadataNameString = SDL_PROP_APP_METADATA_NAME_STRING;
    constexpr auto sdlPropAppMetadataVersionString = SDL_PROP_APP_METADATA_VERSION_STRING;
    constexpr auto sdlPropAppMetadataIdentifierString = SDL_PROP_APP_METADATA_IDENTIFIER_STRING;
    constexpr auto sdlPropAppMetadataCreatorString = SDL_PROP_APP_METADATA_CREATOR_STRING;
    constexpr auto sdlPropAppMetadataCopyrightString = SDL_PROP_APP_METADATA_COPYRIGHT_STRING;
    constexpr auto sdlPropAppMetadataTypeString = SDL_PROP_APP_METADATA_TYPE_STRING;

    void displayErrorMessage(char const* message)
    {
        SDL_ShowSimpleMessageBox(sdlMessageboxError, "Application Error", message, nullptr);
    }
}