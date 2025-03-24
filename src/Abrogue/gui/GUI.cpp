module GUI;

import GameSystems;

void GUI::init()
{
	buttons[eStartGame].init("New game", 120, 30, QuadPool::eMap);
	buttons[eQuitToDesktop].init("Quit to desktop", 113, 31, QuadPool::eMap);

	buttons[ePause].init("Pause[SPACE]", 26, 1, QuadPool::eMap);

	buttons[eHealth].init("Health", 0, 1, QuadPool::eMap);
	buttons[eHealth].setBackgroundColor(Constants::healthBackgroundColor, Constants::healthHoverColor);
	buttons[eNutrition].init("Nutrition", 7, 1, QuadPool::eMap);
	buttons[eNutrition].setBackgroundColor(Constants::nutritionBackgroundColor, Constants::nutritionHoverColor);

	tabButtons[(size_t)TabButton::eDebug].init("Debug[F3]", 0, 35, QuadPool::eMap);
	tabButtons[(size_t)TabButton::eSearch].init("Search[s]", 11, 35, QuadPool::eMap);
	tabButtons[(size_t)TabButton::eDiscoveries].init("Discoveries[D]", 22, 35, QuadPool::eMap);
	tabButtons[(size_t)TabButton::eMenu].init("Menu[ESC]", 38, 35, QuadPool::eMap);

	menu.init();
	discoveries.init();
	debugMenu.init();

	fpsLabel.init("FPS:", 0, 0, QuadPool::eMap, true);
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
	buttons[ePause].setText("Pause[SPACE]");
	buttons[ePause].setVisible(true);
	for(auto& button : tabButtons)
		button.setVisible(true);
}

void GUI::quitToMenu()
{
	buttons[eHealth].setVisible(false);
	buttons[eNutrition].setVisible(false);

	buttons[ePause].setVisible(false);

	for(auto& button : tabButtons)
		button.setVisible(false);

	buttons[eStartGame].setVisible(true);
	buttons[eQuitToDesktop].setVisible(true);
}

void GUI::setPaused(bool paused)
{
	buttons[ePause].setPressed(paused);
	buttons[ePause].setText(paused ? "PAUSED[SPACE]" : "Pause[SPACE]");
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

void GUI::onMenuToggled()
{
	if(state == State::eRunning)
	{
		if(pressedTabButton != TabButton::COUNT)
		{
			tabButtons[(size_t)pressedTabButton].setPressed(false);
			pressedTabButton = TabButton::eMenu;
		}
		tabButtons[(size_t)TabButton::eMenu].setPressed(true);
		onTabButtonPressed(TabButton::eMenu);
	}
	else if(state == State::ePaused)
	{
		if(pressedTabButton != TabButton::COUNT)
		{
			tabButtons[(size_t)pressedTabButton].setPressed(false);
			pressedTabButton = TabButton::COUNT;
		}
		menu.setVisible(false);
		game.setPaused(false);
		state = State::eRunning;
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
		game.setPaused(!buttons[(size_t)type].getPressed());
}

void GUI::onTabButtonPressed(TabButton type)
{
	if(pressedTabButton != type)
	{
		if(pressedTabButton == TabButton::eMenu)
			menu.setVisible(false);
		else if(pressedTabButton == TabButton::eDiscoveries)
			discoveries.setVisible(false);
		else if(pressedTabButton == TabButton::eDebug)
			debugMenu.setVisible(false);
	}

	auto isPressed = tabButtons[(size_t)type].getPressed();
	if(type == TabButton::eMenu)
	{
		menu.setVisible(isPressed);
		game.setPaused(true);
		state = State::ePaused;
	}
	else if(type == TabButton::eDiscoveries)
	{
		discoveries.setVisible(isPressed);
		game.setPaused(true);
		state = State::ePaused;
	}
	else if(type == TabButton::eDebug)
	{
		debugMenu.setVisible(isPressed);
		game.setPaused(true);
		state = State::ePaused;
	}
	else
	{
		game.setPaused(false);
		state = State::eRunning;
	}
}
