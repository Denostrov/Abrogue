export module GUI;

export import Label;

export class GUI
{
public:
	enum ButtonType
	{
		eStartGame,
		eQuitToDesktop,
		eResume,
		eQuitToMenu,
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

	std::array<Label, COUNT> buttons;
	ButtonType hoveredButton{COUNT};

	Label fpsLabel;
	Label healthLabel;
	Label hungerLabel;
};