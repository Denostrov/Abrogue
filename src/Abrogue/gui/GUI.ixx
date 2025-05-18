export module GUI;

export import MainMenu;
export import PlayArea;
export import Menu;
export import Discoveries;
export import DebugMenu;
export import GameOver;

//Class for handling buttons and menus
export class GUI
{
public:
	GUI() = default;
	void init();

	void showPlayArea();
	void showMainMenu();
	void showGameOver(bool winner);

	void updateDraw(double deltaTime);

	void onMouseMoved(std::int64_t x, std::int64_t y);
	void onMousePressed(std::int64_t x, std::int64_t y);

	void onMenuHotkeyPressed();
	void onDebugHotkeyPressed();
	void onDiscoveriesHotkeyPressed();
	void onPauseHotkeyPressed();
	void onStopTimeHotkeyPressed();
	void onStepTimeHotkeyPressed();

	void setFPS(std::int64_t fps);
	void setPlayerHealth(double percentage);
	void setInventory(FixedVector<Item, 20> const& inventory, std::int64_t gold);

private:
	void setCurrentScreen(Screen& newScreen);

	Screen* currentScreen{};	//Screen that's currently active and receiving input
	bool previouslyPaused{};	//Pause state before the popup screen was shown

	//Available screens
	MainMenu mainMenu;
	PlayArea playArea;
	Menu menu;
	Discoveries discoveries;
	DebugMenu debugMenu;
	GameOver gameOver;

	Background popupBackground;
	Label fpsLabel;
};

export inline GUI gui;