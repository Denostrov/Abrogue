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

	void setFullscreen(bool fullscreen);

private:
	SDL_Window* window{};
	std::vector<char const*> requiredExtensions;
};
