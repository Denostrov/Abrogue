export module GUI;

export import Button;

export class GUI
{
public:
	enum ButtonType
	{
		eStartGame,
		eContinue,
		eExit,
		COUNT
	};

	GUI() = default;

	void init();

	void showStartMenu();
	void startGame();

	void onMousePressed(std::uint32_t x, std::uint32_t y);

	void setFPS(std::uint32_t fps);

private:
	std::array<Button, COUNT> buttons;

	Label fpsLabel;
	Label healthLabel;
	Label hungerLabel;
};