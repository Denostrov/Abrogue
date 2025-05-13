module;

#include <SDL3/SDL_scancode.h>

export module InputHandler;

export import std;

//Class for processing user input
export class InputHandler
{
public:
	InputHandler() = default;

	std::pair<float, float> getMousePosition() const;
	void onMouseMoved(float x, float y);
	void onMousePressed(float x, float y);

	void onButtonPressed(SDL_Scancode scancode, bool pressed);
	void onShiftButtonPressed(SDL_Scancode scancode);

private:
	std::array<bool, SDL_Scancode::SDL_SCANCODE_COUNT> pressedButtons{};
};

export inline InputHandler inputHandler;