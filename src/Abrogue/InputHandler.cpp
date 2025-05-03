module;

#include <SDL3/SDL_mouse.h>

module InputHandler;

import GameSystems;

std::pair<float, float> InputHandler::getMousePosition() const
{
	float x{}, y{};
	SDL_GetMouseState(&x, &y);
	return {x, y};
}

void InputHandler::setButtonPressed(SDL_Scancode scancode, bool pressed)
{
	if(pressed)
	{
		if(scancode == SDL_SCANCODE_SPACE)
			gui.togglePause();
		else if(scancode == SDL_SCANCODE_ESCAPE)
			gui.toggleMenu();
		else if(scancode == SDL_SCANCODE_F3)
			gui.toggleDebugOptions();
		else if(scancode == SDL_SCANCODE_KP_7)
			gui.toggleStopTime();
		else if(scancode == SDL_SCANCODE_KP_8)
			gui.toggleStepTime();
	}

	pressedButtons[scancode] = pressed;
}

void InputHandler::setShiftButtonPressed(SDL_Scancode scancode)
{
	if(scancode == SDL_SCANCODE_D) gui.toggleDiscoveries();
}
