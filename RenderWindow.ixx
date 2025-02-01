module;

#include <SDL3/SDL_video.h>

export module RenderWindow;

export import std;

export class RenderWindow
{
public:
	RenderWindow();
	~RenderWindow();

	auto getHasError() const { return hasError; }
	auto getWindow() const { return window; }
	auto const& getRequiredExtensions() const { return requiredExtensions; }

private:
	bool hasError{};
	SDL_Window* window{};
	std::vector<char const*> requiredExtensions{};
};


