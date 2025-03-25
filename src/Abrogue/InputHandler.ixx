module;

#include <SDL3/SDL_scancode.h>

export module InputHandler;

export import std;

export class InputHandler
{
public:
	InputHandler() = default;

	bool getButtonPressed(SDL_Scancode scancode) const { return pressedButtons[scancode]; }
	void setButtonPressed(SDL_Scancode scancode, bool pressed);

private:
	std::array<bool, SDL_Scancode::SDL_SCANCODE_COUNT> pressedButtons{};
};