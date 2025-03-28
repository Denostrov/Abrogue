export module GUI;

export import Screen;
export import MainMenu;
export import PlayArea;
export import Menu;
export import Discoveries;
export import DebugMenu;

export class GUI
{
public:
	GUI() = default;

	void init();

	void startGame();
	void quitToMainMenu();

	void onMouseMoved(std::uint32_t x, std::uint32_t y);
	void onMousePressed(std::uint32_t x, std::uint32_t y);

	void setFPS(std::uint32_t fps);

	void togglePause();
	void toggleMenu();
	void toggleDebugOptions();
	void toggleDiscoveries();

private:
	void setCurrentScreen(Screen& newScreen);

	Screen* currentScreen{};
	bool previouslyPaused{};

	MainMenu mainMenu;
	PlayArea playArea;
	Menu menu;
	Discoveries discoveries;
	DebugMenu debugMenu;

	Label fpsLabel;
};