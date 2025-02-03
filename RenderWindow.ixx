module;

#include <SDL3/SDL_video.h>
#include <SDL3/SDL_vulkan.h>

export module RenderWindow;

export import std;

export class RenderWindow
{
public:
	RenderWindow();
	~RenderWindow();

	bool createSurface(VkInstance instance);

	auto getHasError() const { return hasError; }

	auto const& getRequiredExtensions() const { return requiredExtensions; }
	auto getSurface() const { return surface; }
	std::pair<uint32_t, uint32_t> getFramebufferSize() const;

private:
	bool checkSDLErrorOccured(bool checkValue);

	bool hasError{};
	SDL_Window* window{};
	std::vector<char const*> requiredExtensions{};
	VkInstance instance{};
	VkSurfaceKHR surface{};
};


