export module GameOver;

export import Screen;
export import Background;

export class GameOver : public Screen
{
public:
	GameOver() = default;
	//Initialize labels
	void init();

	void updateDraw(double deltaTime) final;
	void setVisible(bool visible) final;

private:
	Label gameOverLabel;
	Label pressSpaceLabel;

	Background background;
};
