export module GUI;

export import Button;

export class GUI
{
public:
	enum ButtonType
	{
		eStartGame,
		eResume,
		eQuitToMenu,
		COUNT
	};

	GUI() = default;

	void init();

	void showStartMenu();
	void startGame();

	void pauseGame();
	void resumeGame();

	void onMousePressed(std::uint32_t x, std::uint32_t y);

	void setFPS(std::uint32_t fps);

private:
	void onButtonPressed(ButtonType type);

	std::array<Button, COUNT> buttons;

	Label fpsLabel;
	Label healthLabel;
	Label hungerLabel;
};