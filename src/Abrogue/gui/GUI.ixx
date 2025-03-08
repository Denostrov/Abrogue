export module GUI;

export import Button;

export class GUI
{
public:
	GUI() = default;

	void init();
	void startGame();

	void setFPS(std::uint32_t fps);

private:
	Button startGameButton;

	Label fpsLabel;
	Label healthLabel;
	Label hungerLabel;
};