module;

#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_keyboard.h>

module InputHandler;

import RenderEngine;
import GUI;
import Game;

std::pair<float, float> InputHandler::getMousePosition() const
{
	float x{}, y{};
	SDL_GetMouseState(&x, &y);

	auto [width, height] = renderEngine.getFramebufferSize();
	return {x / width * Constants::screenWidth, y / height * Constants::screenHeight};
}

void InputHandler::onMouseMoved(float x, float y)
{
	auto [width, height] = renderEngine.getFramebufferSize();
	gui.onMouseMoved(x / width * Constants::screenWidth, y / height * Constants::screenHeight);
}

void InputHandler::onMousePressed(float x, float y)
{
	auto [width, height] = renderEngine.getFramebufferSize();
	gui.onMousePressed(x / width * Constants::screenWidth, y / height * Constants::screenHeight);
}

void InputHandler::onButtonPressed(SDL_Scancode scancode, bool pressed)
{
	pressedButtons[scancode] = pressed;

	if(pressed)
	{
		if(scancode == SDL_SCANCODE_SPACE)
			gui.onPauseHotkeyPressed();
		else if(scancode == SDL_SCANCODE_ESCAPE)
			gui.onPauseMenuHotkeyPressed();
		else if(scancode == SDL_SCANCODE_F3)
			gui.onDebugHotkeyPressed();
		else if(scancode == SDL_SCANCODE_KP_7)
			gui.onStopTimeHotkeyPressed();
		else if(scancode == SDL_SCANCODE_KP_8)
			gui.onStepTimeHotkeyPressed();
		else if(scancode == SDL_SCANCODE_W || scancode == SDL_SCANCODE_A || scancode == SDL_SCANCODE_S || scancode == SDL_SCANCODE_D)
			game.setPlayerMovement(pressedButtons[SDL_SCANCODE_D] - pressedButtons[SDL_SCANCODE_A], pressedButtons[SDL_SCANCODE_S] - pressedButtons[SDL_SCANCODE_W]);
	}
	else
	{
		if(scancode == SDL_SCANCODE_W || scancode == SDL_SCANCODE_A || scancode == SDL_SCANCODE_S || scancode == SDL_SCANCODE_D)
			game.setPlayerMovement(pressedButtons[SDL_SCANCODE_D] - pressedButtons[SDL_SCANCODE_A], pressedButtons[SDL_SCANCODE_S] - pressedButtons[SDL_SCANCODE_W]);
	}
}

void InputHandler::onShiftButtonPressed(SDL_Scancode scancode)
{
	if(scancode == SDL_SCANCODE_D) 
		gui.onDiscoveriesHotkeyPressed();
}
