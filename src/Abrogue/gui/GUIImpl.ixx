module Abrogue:GUIImpl;

import :GUI;
import :InputHandler;
import :Game;

using namespace std::literals;

void GUI::init()
{
	//Initialize all available screens
	mainMenu.init();
	optionsMenu.init();
	playArea.init();
	pauseMenu.init();
	discoveries.init();
	debugMenu.init();
	gameOver.init();

	fpsLabel.init("FPS:"sv, 0, 0, true);

	setCurrentScreen(ScreenType::eMainMenu);
}
void GUI::showPlayArea()
{
	setCurrentScreen(ScreenType::ePlayArea);
}
void GUI::showMainMenu()
{
	//Restore default GUI state
	playArea.setPaused(false);
	debugMenu.resetToDefault();

	setCurrentScreen(ScreenType::eMainMenu);
}
void GUI::showOptionsMenu()
{
	setCurrentScreen(ScreenType::eOptionsMenu);
}
void GUI::showGameOver(bool winner)
{
	gameOver.setWinner(winner);

	setCurrentScreen(ScreenType::eGameOver);
}
void GUI::updateDraw(double deltaTime)
{
	popupBackground.updateDraw(deltaTime);
}
void GUI::onMouseMoved(std::int64_t x, std::int64_t y)
{
	executeOnScreen(activeScreenType, [x, y](auto& screen) { screen.updateMouseMoved(x, y); });
}
void GUI::onMousePressed(std::int64_t x, std::int64_t y)
{
	//Handle player actions when pressing on the map
	if(activeScreenType == ScreenType::ePlayArea && !playArea.getPaused() && x >= Constants::mapOffset)
		player.onMousePressed(x - Constants::mapOffset, y);

	executeOnScreen(activeScreenType, [x, y](auto& screen) { screen.updateMousePressed(x, y); });
}
void GUI::onPauseMenuHotkeyPressed()
{
	//Requesting the pause menu before the game starts does nothing
	if(activeScreenType == ScreenType::eMainMenu)
		return;

	//Requesting the pause menu after game over returns to the main menu
	if(activeScreenType == ScreenType::eGameOver)
	{
		game.quitToMainMenu();
		return;
	}

	//Close a popup if it's the current screen
	if(isScreenAPopup(activeScreenType))
	{
		playArea.setPaused(previouslyPaused);
		playArea.setTabButtonPressed(PlayArea::TabButtonType::COUNT);
		setCurrentScreen(backgroundScreenType);
		return;
	}

	//Show the pause menu
	playArea.setPaused(true);
	playArea.setTabButtonPressed(PlayArea::TabButtonType::eMenu);
	setCurrentScreen(ScreenType::ePauseMenu);
}
void GUI::onDebugHotkeyPressed()
{
	//Requesting the debug menu only works during play
	if(activeScreenType == ScreenType::eMainMenu || activeScreenType == ScreenType::ePauseMenu || activeScreenType == ScreenType::eGameOver)
		return;

	//Close debug menu if its already open
	if(activeScreenType == ScreenType::eDebugMenu)
	{
		playArea.setPaused(previouslyPaused);
		playArea.setTabButtonPressed(PlayArea::TabButtonType::COUNT);
		setCurrentScreen(backgroundScreenType);
		return;
	}

	//Show debug options
	playArea.setPaused(true);
	playArea.setTabButtonPressed(PlayArea::TabButtonType::eDebug);
	setCurrentScreen(ScreenType::eDebugMenu);
}
void GUI::onDiscoveriesHotkeyPressed()
{
	//Requesting the discoveries menu only works during play
	if(activeScreenType == ScreenType::eMainMenu || activeScreenType == ScreenType::ePauseMenu || activeScreenType == ScreenType::eGameOver)
		return;

	//Close discoveries menu if its already open
	if(activeScreenType == ScreenType::eDiscoveries)
	{
		playArea.setPaused(previouslyPaused);
		playArea.setTabButtonPressed(PlayArea::TabButtonType::COUNT);
		setCurrentScreen(backgroundScreenType);
		return;
	}

	//Show discoveries menu
	playArea.setPaused(true);
	playArea.setTabButtonPressed(PlayArea::TabButtonType::eDiscoveries);
	setCurrentScreen(ScreenType::eDiscoveries);
}
void GUI::onPauseHotkeyPressed()
{
	//Pausing only allowed during active play
	if(activeScreenType != ScreenType::ePlayArea)
		return;

	previouslyPaused = !playArea.getPaused();
	playArea.setPaused(previouslyPaused);
}
void GUI::onStopTimeHotkeyPressed()
{
	//Stopping time not allowed in main menu or during game over
	if(activeScreenType == ScreenType::eMainMenu || activeScreenType == ScreenType::eGameOver)
		return;

	debugMenu.pressButton(DebugMenu::ButtonType::eStopTime);
}
void GUI::onStepTimeHotkeyPressed()
{
	//Stepping time not allowed in main menu or during game over
	if(activeScreenType == ScreenType::eMainMenu || activeScreenType == ScreenType::eGameOver)
		return;

	debugMenu.pressButton(DebugMenu::ButtonType::eStepTime);
}
void GUI::refreshScreens()
{
	optionsMenu.refreshLabels();
	debugMenu.refreshLabels();
	gameOver.refreshLabels();
	playArea.refreshLabels();
}
void GUI::setCurrentScreen(ScreenType screenType)
{
	//Do nothing if the old and new screens are the same
	if(activeScreenType == screenType)
		return;

	auto isOldScreenAPopup = isScreenAPopup(activeScreenType);
	auto isNewScreenAPopup = isScreenAPopup(screenType);

	//Show popup on top of the already visible screen
	//If the old screen was a popup too, simply switch to the new one
	//Else keep the old screen visible in the background
	if(isNewScreenAPopup)
	{
		if(isOldScreenAPopup)
			setScreenVisible(activeScreenType, false);
		else
		{
			popupBackground.setVisible(true);
			backgroundScreenType = activeScreenType;
		}

		setScreenVisible(screenType, true);
	}
	//Hide the popup and its background
	//If the new and background screens are different, hide the old background too
	else if(isOldScreenAPopup)
	{
		popupBackground.setVisible(false);

		setScreenVisible(activeScreenType, false);
		if(screenType != backgroundScreenType)
		{
			setScreenVisible(backgroundScreenType, false);
			setScreenVisible(screenType, true);
		}

		backgroundScreenType = ScreenType::eNone;
	}
	//Hide the old screen and show the new one
	else
	{
		setScreenVisible(activeScreenType, false);
		setScreenVisible(screenType, true);
	}

	activeScreenType = screenType;

	//Refresh mouse position for the newly shown screen
	auto [x, y] = inputHandler.getMousePosition();
	onMouseMoved(x, y);
}
void GUI::setScreenVisible(ScreenType screenType, bool visible)
{
	executeOnScreen(screenType, [visible](auto& screen) { screen.setVisible(visible); });
}
bool GUI::isScreenAPopup(ScreenType screenType) const
{
	return screenType == ScreenType::ePauseMenu || screenType == ScreenType::eDiscoveries || screenType == ScreenType::eDebugMenu ||
		screenType == ScreenType::eGameOver || screenType == ScreenType::eOptionsMenu;
}
void GUI::setFPS(std::int64_t fps)
{
	Array<char, 32> buf{"FPS:"};
	std::to_chars(buf.data + 4, buf.data + 15, fps);

	fpsLabel.setText(buf.data);
}
void GUI::setPlayerHealth(double percentage)
{
	playArea.setPlayerHealth(percentage);
}
void GUI::setInventory(FixedVector<Item, 20> const& inventory, std::int64_t gold)
{
	playArea.updateInventory(inventory, gold);
}