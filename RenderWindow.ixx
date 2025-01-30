module;

#include <SDL3/SDL_video.h>

export module RenderWindow;

export class RenderWindow
{
public:
	RenderWindow();
	~RenderWindow();

	auto getHasError() const { return hasError; }

private:
	bool hasError{};
	SDL_Window* window{};
};


