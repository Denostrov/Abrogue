module;

#include <SDL3/SDL_video.h>
#include <SDL3/SDL_vulkan.h>

export module RenderWindow;

export import std;

export class RenderWindow
{
public:
	RenderWindow() = default;
	~RenderWindow();

	bool initSDL();

	[[nodiscard]] VkSurfaceKHR createSurface(VkInstance instance);

	[[nodiscard]] auto const& getRequiredExtensions() const { return requiredExtensions; }
	[[nodiscard]] std::pair<uint32_t, uint32_t> getFramebufferSize() const;

private:
	bool checkSDLErrorOccured(bool checkValue);

	SDL_Window* window;
	std::vector<char const*> requiredExtensions{};
};
