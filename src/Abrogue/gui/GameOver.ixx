export module GameOver;

export import Screen;

//Class for game over screen
export class GameOver : public Screen
{
public:
	GameOver() = default;
	void init();

	void onButtonPressed(std::size_t index) {}

	void setVisible(bool visible);

	void setWinner(bool winner);

private:
	Label gameOverLabel;
	Label pressSpaceLabel;
};
