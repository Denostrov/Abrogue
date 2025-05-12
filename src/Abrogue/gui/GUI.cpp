module GUI;

import GameSystems;

using namespace std::literals;

void GUI::init()
{
	mainMenu.init();
	playArea.init();
	menu.init();
	discoveries.init();
	debugMenu.init();
	gameOver.init();

	fpsLabel.init("FPS:"sv, 0, 0, QuadPool::eMap, true);

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
	//Show background if not already visible and hide popups
	if(currentScreen == &playArea)
		popupBackground.setVisible(true);
	else
		currentScreen->setVisible(false);

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
	if(currentScreen == &playArea && !playArea.getPaused() && x >= Constants::mapOffset)
		player.onMousePressed(x - Constants::mapOffset, y);

	currentScreen->updateMousePressed(x, y);
}

void GUI::togglePause()
{
	if(currentScreen != &playArea)
		return;

	previouslyPaused = !playArea.getPaused();
	playArea.setPaused(previouslyPaused);
}

void GUI::toggleMenu()
{
	if(currentScreen == &gameOver)
	{
		quitToMainMenu();
		return;
	}

	if(currentScreen == &mainMenu)
		return;

	//Close a popup if its the current screen
	if(currentScreen != &playArea)
	{
		playArea.setPaused(previouslyPaused);

		currentScreen->setVisible(false);
		setCurrentScreen(playArea);
		popupBackground.setVisible(false);

		playArea.setTabButtonPressed(PlayArea::ButtonType::COUNT);
		return;
	}

	//Show options menu
	playArea.setPaused(true);

	menu.setVisible(true);
	setCurrentScreen(menu);
	popupBackground.setVisible(true);

	playArea.setTabButtonPressed(PlayArea::ButtonType::eMenu);
}

void GUI::toggleDebugOptions()
{
	if(currentScreen == &mainMenu || currentScreen == &menu || currentScreen == &gameOver)
		return;

	//Close debug menu if its already open or switch to it if another popup is open
	if(currentScreen != &playArea)
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
		return;
	}

	//Show debug options
	playArea.setPaused(true);

	debugMenu.setVisible(true);
	setCurrentScreen(debugMenu);
	popupBackground.setVisible(true);

	playArea.setTabButtonPressed(PlayArea::ButtonType::eDebug);

}

void GUI::toggleDiscoveries()
{
	if(currentScreen == &mainMenu || currentScreen == &menu || currentScreen == &gameOver)
		return;

	//Close discoveries menu if its already open or switch to it if another popup is open
	if(currentScreen != &playArea)
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
		return;
	}

	//Show discoveries menu
	playArea.setPaused(true);

	discoveries.setVisible(true);
	setCurrentScreen(discoveries);
	popupBackground.setVisible(true);

	playArea.setTabButtonPressed(PlayArea::ButtonType::eDiscoveries);
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

	//Refresh mouse position for the newly shown screen
	auto [x, y] = inputHandler.getMousePosition();
	onMouseMoved(x, y);
}

void GUI::setFPS(std::int64_t fps)
{
	std::array<char, 16> buf{};
	std::to_chars(buf.data(), buf.data() + 15, fps);

	FixedVector<char, 16> fpsString{"FPS:"sv};
	fpsString.append(buf.data());
	fpsLabel.setText(fpsString);
}

void GUI::setPlayerHealth(double percentage)
{
	playArea.setPlayerHealth(percentage);
}
