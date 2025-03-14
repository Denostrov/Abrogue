import GUI;

import GameSystems;

void GUI::init()
{
	buttons[eStartGame].init("New game", 120, 30);
	buttons[eQuitToDesktop].init("Quit to desktop", 113, 31);

	buttons[eEnableDebug].init("Enable debug", 0, 3);
	buttons[eAbandonGame].init("Abandon game", 0, 4);
	buttons[eSaveAndQuit].init("Save and quit", 0, 5);

	buttons[ePause].init("Pause [SPACE]", 26, 1);

	buttons[eHealth].init("Health", 0, 1);
	buttons[eHealth].setBackgroundColor(Constants::healthBackgroundColor, Constants::healthHoverColor);
	buttons[eNutrition].init("Nutrition", 7, 1);
	buttons[eNutrition].setBackgroundColor(Constants::nutritionBackgroundColor, Constants::nutritionHoverColor);

	tabButtons[(size_t)TabButton::eDebug].init("Debug", 0, 35);
	tabButtons[(size_t)TabButton::eInventory].init("Inventory", 8, 35);
	tabButtons[(size_t)TabButton::eDiscoveries].init("Discoveries", 21, 35);
	tabButtons[(size_t)TabButton::eMenu].init("Menu [ESC]", 36, 35);

	fpsLabel.init("FPS:", 0, 0, true);
}

void GUI::showStartMenu()
{
	buttons[eStartGame].setVisible(true);
	buttons[eQuitToDesktop].setVisible(true);
}

void GUI::startGame()
{
	buttons[eStartGame].setVisible(false);
	buttons[eQuitToDesktop].setVisible(false);

	buttons[eHealth].setVisible(true);
	buttons[eNutrition].setVisible(true);
	buttons[ePause].setText("Pause [SPACE]");
	buttons[ePause].setVisible(true);
	for(auto& button : tabButtons)
		button.setVisible(true);
}

void GUI::quitToMenu()
{
	buttons[eEnableDebug].setVisible(false);
	buttons[eAbandonGame].setVisible(false);
	buttons[eSaveAndQuit].setVisible(false);
	buttons[eHealth].setVisible(false);
	buttons[eNutrition].setVisible(false);

	buttons[ePause].setVisible(false);

	for(auto& button : tabButtons)
		button.setVisible(false);

	buttons[eStartGame].setVisible(true);
	buttons[eQuitToDesktop].setVisible(true);
}

void GUI::pauseGame()
{
	buttons[ePause].setText("PAUSED [SPACE]");
}

void GUI::resumeGame()
{
	buttons[ePause].setText("Pause [SPACE]");
}

void GUI::onMouseMoved(std::uint32_t x, std::uint32_t y)
{
	auto updateHoveredButton = [this](Label* newHoveredButton)
	{
		if(newHoveredButton != hoveredButton)
		{
			if(hoveredButton)
				hoveredButton->setHovered(false);
			hoveredButton = newHoveredButton;
		}
	};

	for(size_t i = 0; i < buttons.size(); i++)
	{
		if(buttons[i].checkCollision(x, y))
		{
			buttons[i].setHovered(true);
			updateHoveredButton(&buttons[i]);
			return;
		}
	}
	for(size_t i = 0; i < tabButtons.size(); i++)
	{
		if(tabButtons[i].checkCollision(x, y))
		{
			tabButtons[i].setHovered(true);
			updateHoveredButton(&tabButtons[i]);
			return;
		}
	}
	updateHoveredButton(nullptr);
}

void GUI::onMousePressed(std::uint32_t x, std::uint32_t y)
{
	for(size_t i = 0; i < buttons.size(); i++)
	{
		if(buttons[i].checkCollision(x, y))
		{
			onButtonPressed((ButtonType)i);
			return;
		}
	}

	auto updatePressedButton = [this](TabButton newPressedButton)
	{
		if(newPressedButton != pressedTabButton)
		{
			if(pressedTabButton != TabButton::COUNT)
				tabButtons[(size_t)pressedTabButton].setPressed(false);
			pressedTabButton = newPressedButton;
		}
	};
	for(size_t i = 0; i < tabButtons.size(); i++)
	{
		if(tabButtons[i].checkCollision(x, y))
		{
			tabButtons[i].setPressed(true);
			onTabButtonPressed((TabButton)i);
			updatePressedButton((TabButton)i);
			return;
		}
	}
}

void GUI::setFPS(std::uint32_t fps)
{
	std::array<char, 16> fpsString{"FPS:"};
	std::to_chars(fpsString.data() + 4, fpsString.data() + 14, fps);
	fpsLabel.setText(fpsString.data());
}

void GUI::onButtonPressed(ButtonType type)
{
	if(type == eStartGame)
		game.startGame();
	else if(type == eQuitToDesktop)
		game.quitToDesktop();
	else if(type == ePause)
	{
		buttons[(size_t)type].togglePressed();
		if(game.getState() == Game::ePaused)
			game.resumeGame();
		else
			game.pauseGame();
	}
	else if(type == eSaveAndQuit)
		game.quitToMenu();
}

void GUI::onTabButtonPressed(TabButton type)
{
	if(pressedTabButton != type)
	{
		if(pressedTabButton == TabButton::eMenu)
		{
			buttons[eEnableDebug].setVisible(false);
			buttons[eAbandonGame].setVisible(false);
			buttons[eSaveAndQuit].setVisible(false);
		}
	}

	if(type == TabButton::eMenu)
	{
		auto isPressed = tabButtons[(size_t)type].getPressed();
		buttons[eEnableDebug].setVisible(isPressed);
		buttons[eAbandonGame].setVisible(isPressed);
		buttons[eSaveAndQuit].setVisible(isPressed);
	}
}
