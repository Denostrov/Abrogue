module GUI;

import GameSystems;

void GUI::init()
{
	mainMenu.init();
	playArea.init();
	menu.init();
	discoveries.init();
	debugMenu.init();

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

	mainMenu.setVisible(true);
	setCurrentScreen(mainMenu);
	game.quitToMainMenu();
}

void GUI::onMouseMoved(std::uint32_t x, std::uint32_t y)
{
	currentScreen->updateMouseMoved(x, y);
}

void GUI::onMousePressed(std::uint32_t x, std::uint32_t y)
{
	currentScreen->updateMousePressed(x, y);
}

void GUI::togglePause()
{
	if(currentScreen == &playArea)
	{
		previouslyPaused = !playArea.getPaused();
		playArea.setPaused(previouslyPaused);
	}
}

void GUI::toggleMenu()
{
	if(currentScreen == &mainMenu)
		return;

	if(currentScreen == &menu)
	{
		menu.setVisible(false);
		setCurrentScreen(playArea);
		playArea.setPaused(previouslyPaused);
	}
	else
	{
		if(currentScreen != &playArea)
			currentScreen->setVisible(false);
		else
			playArea.setPaused(true);

		menu.setVisible(true);
		setCurrentScreen(menu);
	}
}

void GUI::toggleDebugOptions()
{
	if(currentScreen == &mainMenu)
		return;

	if(currentScreen == &debugMenu)
	{
		debugMenu.setVisible(false);
		setCurrentScreen(playArea);
		playArea.setPaused(previouslyPaused);
	}
	else
	{
		if(currentScreen != &playArea)
			currentScreen->setVisible(false);
		else
			playArea.setPaused(true);

		debugMenu.setVisible(true);
		setCurrentScreen(debugMenu);
	}
}

void GUI::toggleDiscoveries()
{
	if(currentScreen == &mainMenu)
		return;

	if(currentScreen == &discoveries)
	{
		discoveries.setVisible(false);
		setCurrentScreen(playArea);
		playArea.setPaused(previouslyPaused);
	}
	else
	{
		if(currentScreen != &playArea)
			currentScreen->setVisible(false);
		else
			playArea.setPaused(true);

		discoveries.setVisible(true);
		setCurrentScreen(discoveries);
	}
}

void GUI::setCurrentScreen(Screen& newScreen)
{
	newScreen.recheckHoverPosition();
	currentScreen = &newScreen;
}

void GUI::setFPS(std::uint32_t fps)
{
	std::array<char, 16> fpsString{"FPS:"};
	std::to_chars(fpsString.data() + 4, fpsString.data() + 14, fps);
	fpsLabel.setText(fpsString.data());
}
