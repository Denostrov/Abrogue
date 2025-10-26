module Abrogue:InputHandlerImpl;

import :InputHandler;
import :RenderWindow;
import :GUI;
import :Configuration;
import :Game;

std::pair<float, float> InputHandler::getMousePosition() const
{
	float x{}, y{};
	SDL_GetMouseState(&x, &y);

	auto [width, height] = renderWindow.getWindowSize();
	return {x / width * Constants::screenWidth, y / height * Constants::screenHeight};
}
void InputHandler::onMouseMoved(float x, float y)
{
	auto [width, height] = renderWindow.getWindowSize();
	gui.onMouseMoved(x / width * Constants::screenWidth, y / height * Constants::screenHeight);
}
void InputHandler::onMousePressed(std::uint8_t buttonIndex, float x, float y)
{
	if(changingControlType != InputControlType::COUNT)
	{
		configuration.setInputControlScancode(changingControlType, (SDL_Scancode)(buttonIndex + 300));
		changingControlType = InputControlType::COUNT;
		return;
	}

	auto inputControl = configuration.getInputControlFromScancode((SDL_Scancode)(buttonIndex + 300));

	if(inputControl == InputControlType::eAttack)
	{
		auto [width, height] = renderWindow.getWindowSize();
		gui.onMousePressed(x / width * Constants::screenWidth, y / height * Constants::screenHeight);
	}
}
void InputHandler::onButtonPressed(SDL_Scancode scancode, bool pressed)
{
	pressedButtons[scancode] = pressed;

	if(pressed)
	{
		if(changingControlType != InputControlType::COUNT)
		{
			configuration.setInputControlScancode(changingControlType, scancode == SDL_SCANCODE_ESCAPE ? configuration.getScancodeFromInputControl(changingControlType) : scancode);
			changingControlType = InputControlType::COUNT;
			return;
		}

        if (scancode == SDL_SCANCODE_ESCAPE)
        {
            gui.onPauseMenuHotkeyPressed();
            return;
        }

	    auto inputControl = configuration.getInputControlFromScancode(scancode);

		if(inputControl == InputControlType::ePause)
			gui.onPauseHotkeyPressed();
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
			std::int64_t moveRight = pressedButtons[configuration.getScancodeFromInputControl(InputControlType::eMoveRight)];
			std::int64_t moveLeft = pressedButtons[configuration.getScancodeFromInputControl(InputControlType::eMoveLeft)];
			std::int64_t moveDown = pressedButtons[configuration.getScancodeFromInputControl(InputControlType::eMoveDown)];
			std::int64_t moveUp = pressedButtons[configuration.getScancodeFromInputControl(InputControlType::eMoveUp)];
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
			std::int64_t moveRight = pressedButtons[configuration.getScancodeFromInputControl(InputControlType::eMoveRight)];
			std::int64_t moveLeft = pressedButtons[configuration.getScancodeFromInputControl(InputControlType::eMoveLeft)];
			std::int64_t moveDown = pressedButtons[configuration.getScancodeFromInputControl(InputControlType::eMoveDown)];
			std::int64_t moveUp = pressedButtons[configuration.getScancodeFromInputControl(InputControlType::eMoveUp)];
			game.setPlayerMovement(moveRight - moveLeft, moveDown - moveUp);
		}
	}
}