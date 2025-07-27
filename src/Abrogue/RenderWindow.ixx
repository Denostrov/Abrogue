module;

#include <SDL3/SDL_video.h>
#include <SDL3/SDL_vulkan.h>

export module RenderWindow;

export import std;

//Class for handling SDL initialization and window creation
export class RenderWindow
{
public:
	RenderWindow() = default;
	~RenderWindow();

	//Initialize SDL and create a window
	[[nodiscard]] bool initSDL();

	//Create Vulkan surface from window
	[[nodiscard]] VkSurfaceKHR createSurface(VkInstance instance);

	//Get names of extensions required for surface creation
	[[nodiscard]] auto const& getRequiredExtensions() const { return requiredExtensions; }
	//Get size of window in pixels
	[[nodiscard]] std::pair<uint32_t, uint32_t> getWindowSize() const;

	[[nodiscard]] bool getIsMaximized() const { return SDL_GetWindowFlags(window) & SDL_WINDOW_MAXIMIZED; }
	void setIsMaximized(bool maximized) {
		
		if(maximized) SDL_MaximizeWindow(window);
		else SDL_RestoreWindow(window);
		SDL_SyncWindow(window);
	}

	[[nodiscard]] bool getIsFullscreen() const { return SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN; }
	void setIsFullscreen(bool fullscreen)
	{
		SDL_SetWindowFullscreen(window, fullscreen);
		SDL_SyncWindow(window);
	}

private:
	SDL_Window* window{};
	std::vector<char const*> requiredExtensions;
};

export inline RenderWindow renderWindow;