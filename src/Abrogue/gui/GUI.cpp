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
	playArea.setVisible(false);
	menu.setVisible(false);
	gameOver.setVisible(false);

	mainMenu.setVisible(true);
	setCurrentScreen(mainMenu);
	game.quitToMainMenu();
}

void GUI::triggerGameOver()
{
	if(currentScreen != &playArea)
		currentScreen->setVisible(false);

	gameOver.setVisible(true);
	setCurrentScreen(gameOver);
}

void GUI::onMouseMoved(std::uint32_t x, std::uint32_t y)
{
	currentScreen->updateMouseMoved(x, y);
}

void GUI::onMousePressed(std::uint32_t x, std::uint32_t y)
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
		playArea.setTabButtonPressed(PlayArea::ButtonType::eMenu);
	}
	else
	{
		currentScreen->setVisible(false);
		setCurrentScreen(playArea);
		playArea.setPaused(previouslyPaused);
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
		playArea.setTabButtonPressed(PlayArea::ButtonType::eDebug);
	}
	else
	{
		currentScreen->setVisible(false);
		if(currentScreen == &debugMenu)
		{
			setCurrentScreen(playArea);
			playArea.setPaused(previouslyPaused);
			playArea.setTabButtonPressed(PlayArea::ButtonType::COUNT);
		}
		else
		{
			debugMenu.setVisible(true);
			setCurrentScreen(debugMenu);
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
		playArea.setTabButtonPressed(PlayArea::ButtonType::eDiscoveries);
	}
	else
	{
		currentScreen->setVisible(false);
		if(currentScreen == &discoveries)
		{
			setCurrentScreen(playArea);
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
