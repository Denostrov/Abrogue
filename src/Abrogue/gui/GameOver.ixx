export module GameOver;

export import Screen;

//Class for game over screen
export class GameOver : public ScreenComponent<GameOver>
{
public:
	GameOver() = default;
	void init();

	void setVisible(bool visible);
	void setWinner(bool winner);

private:
	Label gameOverLabel;
	Label pressSpaceLabel;
};
