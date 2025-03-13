export module GUI;

export import Label;

export class GUI
{
public:
	enum ButtonType
	{
		eStartGame,
		eQuitToDesktop,
		eEnableDebug,
		eAbandonGame,
		eSaveAndQuit,
		ePause,
		eHealth,
		eNutrition,
		COUNT
	};

	enum class TabButton
	{
		eDebug,
		eInventory,
		eDiscoveries,
		eMenu,
		COUNT
	};

	GUI() = default;

	void init();

	void showStartMenu();
	void startGame();
	void quitToMenu();

	void pauseGame();
	void resumeGame();

	void onMouseMoved(std::uint32_t x, std::uint32_t y);
	void onMousePressed(std::uint32_t x, std::uint32_t y);

	void setFPS(std::uint32_t fps);

private:
	void onButtonPressed(ButtonType type);
	void onTabButtonPressed(TabButton type);

	std::array<Label, COUNT> buttons;
	std::array<Label, (size_t)TabButton::COUNT> tabButtons;
	Label* hoveredButton{};

	bool isMenuToggled{};

	Label fpsLabel;
};