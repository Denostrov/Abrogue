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
		if(changingControlType != InputControlType::COUNT)
		{
			configuration.setInputControlScancode(changingControlType, scancode);
			changingControlType = InputControlType::COUNT;
			gui.refreshScreens();
			return;
		}

		auto inputControl = configuration.getInputControlFromScancode(scancode);

		if(inputControl == InputControlType::ePause)
			gui.onPauseHotkeyPressed();
		else if(inputControl == InputControlType::eMenu)
			gui.onPauseMenuHotkeyPressed();
		else if(inputControl == InputControlType::eDiscoveries)
			gui.onDiscoveriesHotkeyPressed();
		else if(inputControl == InputControlType::eDebug)
			gui.onDebugHotkeyPressed();
		else if(inputControl == InputControlType::eStopTime)
			gui.onStopTimeHotkeyPressed();
		else if(inputControl == InputControlType::eStepTime)
			gui.onStepTimeHotkeyPressed();
		else if(inputControl == InputControlType::eMoveUp ||
				inputControl == InputControlType::eMoveLeft ||
				inputControl == InputControlType::eMoveDown ||
				inputControl == InputControlType::eMoveRight)
		{
			int64_t moveRight = pressedButtons[configuration.getScancodeFromInputControl(InputControlType::eMoveRight)];
			int64_t moveLeft = pressedButtons[configuration.getScancodeFromInputControl(InputControlType::eMoveLeft)];
			int64_t moveDown = pressedButtons[configuration.getScancodeFromInputControl(InputControlType::eMoveDown)];
			int64_t moveUp = pressedButtons[configuration.getScancodeFromInputControl(InputControlType::eMoveUp)];
			game.setPlayerMovement(moveRight - moveLeft, moveDown - moveUp);
		}
	}
	else
	{
		auto inputControl = configuration.getInputControlFromScancode(scancode);

		if(inputControl == InputControlType::eMoveUp ||
		   inputControl == InputControlType::eMoveLeft ||
		   inputControl == InputControlType::eMoveDown ||
		   inputControl == InputControlType::eMoveRight)
		{
			int64_t moveRight = pressedButtons[configuration.getScancodeFromInputControl(InputControlType::eMoveRight)];
			int64_t moveLeft = pressedButtons[configuration.getScancodeFromInputControl(InputControlType::eMoveLeft)];
			int64_t moveDown = pressedButtons[configuration.getScancodeFromInputControl(InputControlType::eMoveDown)];
			int64_t moveUp = pressedButtons[configuration.getScancodeFromInputControl(InputControlType::eMoveUp)];
			game.setPlayerMovement(moveRight - moveLeft, moveDown - moveUp);
		}
	}
}
