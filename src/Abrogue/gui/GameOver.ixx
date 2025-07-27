export module GameOver;

export import Screen;

enum class LabelType
{
	eGameOver,
	ePressSpace,
	COUNT
};

//Class for game over screen
export class GameOver : public ScreenComponent<GameOver, LabelType>
{
public:
	GameOver() = default;
	void init();

	void setWinner(bool winner);

	void refreshLabels();
};
