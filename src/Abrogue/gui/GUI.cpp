module GUI;

import GameSystems;

void GUI::init()
{
	mainMenu.init();
	playArea.init();
	menu.init();
	discoveries.init();
	debugMenu.init();
	gameOver.init();

	fpsLabel.init("FPS:", 0, 0, QuadPool::eMap, true);

	mainMenu.setVisible(true);
	setCurrentScreen(mainMenu);
}

void GUI::startGame()
{
	mainMenu.setVisible(false);

	playArea.setVisible(true);
	setCurrentScreen(playArea);
}

void GUI::quitToMainMenu()
{
	//Restore default GUI state
	playArea.setPaused(false);
	debugMenu.resetToDefault();
	popupBackground.setVisible(false);

	//Hide all visible screens
	currentScreen->setVisible(false);
	playArea.setVisible(false);

	mainMenu.setVisible(true);
	setCurrentScreen(mainMenu);

	game.quitToMainMenu();
}

void GUI::triggerGameOver()
{
	currentScreen->setVisible(currentScreen != &playArea);

	popupBackground.setVisible(currentScreen == &playArea);

	gameOver.setVisible(true);
	setCurrentScreen(gameOver);
}

void GUI::updateDraw(double deltaTime)
{
	currentScreen->updateDraw(deltaTime);
	popupBackground.updateDraw(deltaTime);
}

void GUI::onMouseMoved(std::int64_t x, std::int64_t y)
{
	currentScreen->updateMouseMoved(x, y);
}

void GUI::onMousePressed(std::int64_t x, std::int64_t y)
{
	if(currentScreen == &playArea && !playArea.getPaused() && x >= 48)
	{
		player.onMousePressed(x - 48, y);
	}
	currentScreen->updateMousePressed(x, y);
}

void GUI::togglePause()
{
	if(currentScreen == &gameOver)
	{
		quitToMainMenu();
		return;
	}

	if(currentScreen != &playArea)
		return;

	previouslyPaused = !playArea.getPaused();
	playArea.setPaused(previouslyPaused);
}

void GUI::toggleMenu()
{
	if(currentScreen == &mainMenu || currentScreen == &gameOver)
		return;

	if(currentScreen == &playArea)
	{
		playArea.setPaused(true);

		menu.setVisible(true);
		setCurrentScreen(menu);
		popupBackground.setVisible(true);

		playArea.setTabButtonPressed(PlayArea::ButtonType::eMenu);
	}
	else
	{
		playArea.setPaused(previouslyPaused);

		currentScreen->setVisible(false);
		setCurrentScreen(playArea);
		popupBackground.setVisible(false);

		playArea.setTabButtonPressed(PlayArea::ButtonType::COUNT);
	}
}

void GUI::toggleDebugOptions()
{
	if(currentScreen == &mainMenu || currentScreen == &menu || currentScreen == &gameOver)
		return;

	if(currentScreen == &playArea)
	{
		playArea.setPaused(true);

		debugMenu.setVisible(true);
		setCurrentScreen(debugMenu);
		popupBackground.setVisible(true);

		playArea.setTabButtonPressed(PlayArea::ButtonType::eDebug);
	}
	else
	{
		currentScreen->setVisible(false);
		if(currentScreen == &debugMenu)
		{
			setCurrentScreen(playArea);
			popupBackground.setVisible(false);

			playArea.setPaused(previouslyPaused);
			playArea.setTabButtonPressed(PlayArea::ButtonType::COUNT);
		}
		else
		{
			setCurrentScreen(debugMenu);
			debugMenu.setVisible(true);

			playArea.setTabButtonPressed(PlayArea::ButtonType::eDebug);
		}
	}
}

void GUI::toggleDiscoveries()
{
	if(currentScreen == &mainMenu || currentScreen == &menu || currentScreen == &gameOver)
		return;

	if(currentScreen == &playArea)
	{
		playArea.setPaused(true);

		discoveries.setVisible(true);
		setCurrentScreen(discoveries);
		popupBackground.setVisible(true);

		playArea.setTabButtonPressed(PlayArea::ButtonType::eDiscoveries);
	}
	else
	{
		currentScreen->setVisible(false);
		if(currentScreen == &discoveries)
		{
			setCurrentScreen(playArea);
			popupBackground.setVisible(false);

			playArea.setPaused(previouslyPaused);
			playArea.setTabButtonPressed(PlayArea::ButtonType::COUNT);
		}
		else
		{
			discoveries.setVisible(true);
			setCurrentScreen(discoveries);
			playArea.setTabButtonPressed(PlayArea::ButtonType::eDiscoveries);
		}
	}
}

void GUI::toggleStopTime()
{
	if(currentScreen == &mainMenu || currentScreen == &gameOver)
		return;

	debugMenu.toggleStopTime();
}

void GUI::toggleStepTime()
{
	if(currentScreen == &mainMenu || currentScreen == &gameOver)
		return;

	debugMenu.toggleStepTime();
}

void GUI::setCurrentScreen(Screen& newScreen)
{
	currentScreen = &newScreen;
	auto [x, y] = inputHandler.getMousePosition();
	onMouseMoved(x, y);
}

void GUI::setFPS(std::uint32_t fps)
{
	std::array<char, 16> fpsString{"FPS:"};
	std::to_chars(fpsString.data() + 4, fpsString.data() + 14, fps);
	fpsLabel.setText(fpsString.data());
}

void GUI::setPlayerHealth(double percentage)
{
	playArea.setPlayerHealth(percentage);
}
