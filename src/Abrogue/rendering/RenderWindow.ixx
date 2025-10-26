module Abrogue:RenderWindow;

import :Constants;

using namespace std::literals;

/*
 * RenderWindow - class for handling SDL initialization and window creation
 */
class RenderWindow
{
public:
    RenderWindow() = default;
    ~RenderWindow();

    //Initialize SDL and create a window
    [[nodiscard]] bool init();

    //Create Vulkan surface from window
    [[nodiscard]] VkSurfaceKHR createSurface(VkInstance instance) const;

    //Get the window size in pixels
    [[nodiscard]] std::pair<std::int64_t, std::int64_t> getWindowSize() const;

    [[nodiscard]] auto getRequiredExtensions() const { return requiredExtensions; }
    [[nodiscard]] bool getIsMaximized() const { return SDL_GetWindowFlags(window) & sdlWindowMaximized; }
    [[nodiscard]] bool getIsFullscreen() const { return SDL_GetWindowFlags(window) & sdlWindowFullscreen; }

    //Maximize the window and sync changes
    void setIsMaximized(bool maximized) const;
    //Fullscreen the window and sync changes
    void setIsFullscreen(bool fullscreen) const;

private:
    [[nodiscard]] static bool checkError(bool value);

    SDL_Window* window{};
    std::span<char const* const> requiredExtensions{};
};
inline RenderWindow renderWindow;

