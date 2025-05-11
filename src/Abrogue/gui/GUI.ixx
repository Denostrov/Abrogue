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

	void startGame();
	void quitToMainMenu();
	void triggerGameOver();

	void updateDraw(double deltaTime);

	void onMouseMoved(std::int64_t x, std::int64_t y);
	void onMousePressed(std::int64_t x, std::int64_t y);

	void setFPS(std::int64_t fps);
	void setPlayerHealth(double percentage);

	void togglePause();
	void toggleMenu();
	void toggleDebugOptions();
	void toggleDiscoveries();

	void toggleStopTime();
	void toggleStepTime();

private:
	void setCurrentScreen(Screen& newScreen);

	Screen* currentScreen{};
	bool previouslyPaused{};

	MainMenu mainMenu;
	PlayArea playArea;
	Menu menu;
	Discoveries discoveries;
	DebugMenu debugMenu;
	GameOver gameOver;

	Background popupBackground;

	Label fpsLabel;
};
