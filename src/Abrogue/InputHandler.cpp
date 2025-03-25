module InputHandler;

import GameSystems;

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
	}

	pressedButtons[scancode] = pressed;
}