module Abrogue:GUI;

import :Item;
import :Background;
import :Label;
import :MainMenu;
import :OptionsMenu;
import :PlayArea;
import :PauseMenu;
import :Discoveries;
import :DebugMenu;
import :GameOver;

/*
 * GUI - class for handling transitions between screens
 */
class GUI
{
	//Enum for different screen types
	enum class ScreenType
	{
		eNone,
		eMainMenu,
		eOptionsMenu,
		ePlayArea,
		ePauseMenu,
		eDiscoveries,
		eDebugMenu,
		eGameOver
	};

public:
	GUI() {}
	void init();

	void showPlayArea();
	void showMainMenu();
	void showOptionsMenu();
	void showGameOver(bool winner);

	void updateDraw(double deltaTime);

	void onMouseMoved(std::int64_t x, std::int64_t y);
	void onMousePressed(std::int64_t x, std::int64_t y);

	void onPauseMenuHotkeyPressed();
	void onDebugHotkeyPressed();
	void onDiscoveriesHotkeyPressed();
	void onPauseHotkeyPressed();
	void onStopTimeHotkeyPressed();
	void onStepTimeHotkeyPressed();

	void refreshScreens();

	void setFPS(std::int64_t fps);
	void setPlayerHealth(double percentage);
	void setInventory(FixedVector<Item, 20> const& inventory, std::int64_t gold);

private:
	void setCurrentScreen(ScreenType screenType);
	void setScreenVisible(ScreenType screenType, bool visible);
	bool isScreenAPopup(ScreenType screenType) const;

	void executeOnScreen(ScreenType screenType, auto func)
	{
		switch(screenType)
		{
			case ScreenType::eMainMenu:
				func(mainMenu);
				break;
			case ScreenType::eOptionsMenu:
			 	func(optionsMenu);
			 	break;
			case ScreenType::ePlayArea:
				func(playArea);
				break;
			case ScreenType::ePauseMenu:
				func(pauseMenu);
				break;
			case ScreenType::eDiscoveries:
				func(discoveries);
				break;
			case ScreenType::eDebugMenu:
				func(debugMenu);
				break;
			case ScreenType::eGameOver:
				func(gameOver);
				break;
			default:
				break;
		}
	}

	ScreenType activeScreenType{ScreenType::eNone};		//Screen that's currently active and receiving input
	ScreenType backgroundScreenType{ScreenType::eNone};	//Screen that's behind the currently active screen
	bool previouslyPaused{};							//Pause state before the popup screen was shown

	//Available screens
	MainMenu mainMenu;
    OptionsMenu optionsMenu;
	PlayArea playArea;
    PauseMenu pauseMenu;
	Discoveries discoveries;
	DebugMenu debugMenu;
	GameOver gameOver;

	Background popupBackground;	//Semi transparent black box behind popup screens
	Label<QuadLayer::eMap> fpsLabel;				//Debug label for showing fps
};
inline GUI gui;