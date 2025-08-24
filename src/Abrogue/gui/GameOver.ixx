export module GameOver;

export import Screen;

//Enum for game over labels
enum class LabelType
{
	eGameOver,
	ePressSpace,
	COUNT
};

//Class for a popup screen with game over text
export class GameOver : public ScreenComponent<GameOver, LabelType>
{
public:
	GameOver() = default;
	void init();

	void setWinner(bool winner);

	void refreshLabels();
};
