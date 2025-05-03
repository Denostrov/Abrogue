export module GameOver;

export import Screen;

export class GameOver : public Screen
{
public:
	GameOver() = default;
	//Initialize labels
	void init();

	void setVisible(bool visible) final;

private:
	Label gameOverLabel;
	Label pressSpaceLabel;

	QuadPool::Reference backgroundQuad;
};
