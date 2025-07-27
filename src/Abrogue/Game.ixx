export module Game;

export import std;

export class Game
{
public:
	enum State
	{
		eNotStarted,
		eRunning,
		ePaused,
		eFinished
	};

	bool init();
	bool update();
	void advanceStep();

	void startGame();
	void quitToMainMenu();
	void quitToDesktop();

	void setPaused(bool paused);
	void setSpeedMultiplier(double speed);
	void setPlayerMovement(std::int64_t movementX, std::int64_t movementY);

	auto getShouldExit() const { return state == eFinished; }

private:
	void resetTickTimer();

	bool updateDraw(double deltaTime) const; 

	std::uint64_t currentTick{};
	std::uint64_t lastUpdateTime{};
	double speedMultiplier{1.0};

	std::uint64_t framesDrawn{};
	std::uint64_t lastFPSLogTime{};

	State state{};
};

export inline Game game;