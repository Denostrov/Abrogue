export module GameOver;

export import Screen;

//Class for game over screen
export class GameOver : public Screen
{
public:
	GameOver() = default;
	void init();

	void setVisible(bool visible) final;

	void setWinner(bool winner);

private:
	Label gameOverLabel;
	Label pressSpaceLabel;
};
