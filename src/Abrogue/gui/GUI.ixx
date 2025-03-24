export module GUI;

export import Menu;
export import Discoveries;
export import DebugMenu;

export class GUI
{
public:
	enum ButtonType
	{
		eStartGame,
		eQuitToDesktop,
		ePause,
		eHealth,
		eNutrition,
		COUNT
	};

	enum class TabButton
	{
		eDebug,
		eSearch,
		eDiscoveries,
		eMenu,
		COUNT
	};

	enum class State
	{
		eRunning,
		ePaused
	};

	GUI() = default;

	void init();

	void showStartMenu();
	void startGame();
	void quitToMenu();

	void setPaused(bool paused);

	void onMouseMoved(std::uint32_t x, std::uint32_t y);
	void onMousePressed(std::uint32_t x, std::uint32_t y);

	void onMenuToggled();

	void setFPS(std::uint32_t fps);

private:
	void onButtonPressed(ButtonType type);
	void onTabButtonPressed(TabButton type);

	State state{State::eRunning};

	std::array<Label, COUNT> buttons;
	std::array<Label, (size_t)TabButton::COUNT> tabButtons;
	Label* hoveredButton{};
	TabButton pressedTabButton{TabButton::COUNT};

	Menu menu;
	Discoveries discoveries;
	DebugMenu debugMenu;

	Label fpsLabel;
};