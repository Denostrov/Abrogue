module;

#include <SDL3/SDL_scancode.h>

export module InputHandler;

export import Configuration;

//Class for processing user input
export class InputHandler
{
public:
	InputHandler() = default;

	void setChangingControlType(InputControlType type) { changingControlType = type; }

	std::pair<float, float> getMousePosition() const;
	void onMouseMoved(float x, float y);
	void onMousePressed(float x, float y);

	void onButtonPressed(SDL_Scancode scancode, bool pressed);

private:
	std::array<bool, SDL_Scancode::SDL_SCANCODE_COUNT> pressedButtons{};
	InputControlType changingControlType{InputControlType::COUNT};
};

export inline InputHandler inputHandler;