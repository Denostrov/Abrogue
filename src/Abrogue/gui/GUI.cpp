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

	state = State::eNormal;
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

	state = State::eMainMenu;
}

void GUI::setPaused(bool paused)
{
	logger.extraAssert(state != State::eMainMenu, "Paused while the game is not running");

	buttons[ePause].setPressed(paused);
	buttons[ePause].setText(paused ? "PAUSED[SPACE]" : "Pause[SPACE]");
}

void GUI::onMouseMoved(std::uint32_t x, std::uint32_t y)
{
	if(state == State::eMainMenu || state == State::eNormal)
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
}

void GUI::onMousePressed(std::uint32_t x, std::uint32_t y)
{
	if(state == State::eMainMenu || state == State::eNormal)
	{
		for(size_t i = 0; i < buttons.size(); i++)
		{
			if(buttons[i].checkCollision(x, y))
			{
				onButtonPressed((ButtonType)i);
				return;
			}
		}

		for(size_t i = 0; i < tabButtons.size(); i++)
		{
			if(tabButtons[i].checkCollision(x, y))
			{
				setPressedTabButton(pressedTabButton == (TabButton)i ? TabButton::COUNT : (TabButton)i);
				return;
			}
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
		game.setPaused(!buttons[(size_t)type].getPressed());
}

void GUI::setPressedTabButton(TabButton type)
{
	if(pressedTabButton == type)
		return;

	if(pressedTabButton != TabButton::COUNT)
	{
		if(pressedTabButton == TabButton::eMenu)
			menu.setVisible(false);
		else if(pressedTabButton == TabButton::eDiscoveries)
			discoveries.setVisible(false);
		else if(pressedTabButton == TabButton::eDebug)
			debugMenu.setVisible(false);

		tabButtons[(size_t)pressedTabButton].setPressed(false);
	}
	pressedTabButton = type;

	if(type != TabButton::COUNT)
	{
		if(type == TabButton::eMenu)
		{
			menu.setVisible(true);
			game.setPaused(true);
			state = State::eMenu;
		}
		else if(type == TabButton::eDiscoveries)
		{
			discoveries.setVisible(true);
			game.setPaused(true);
			state = State::eDiscoveries;
		}
		else if(type == TabButton::eDebug)
		{
			debugMenu.setVisible(true);
			game.setPaused(true);
			state = State::eDebug;
		}

		tabButtons[(size_t)type].setPressed(true);
	}
	else
	{
		game.setPaused(false);
		state = State::eNormal;
	}
}