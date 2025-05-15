export module Game;

export import Enemy;
export import Map;

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
	auto const& getRandomRoom() const { return map.getRandomRoom(); }
	bool getTileSolid(std::int32_t x, std::int32_t y) const { return map.getTileSolid(x, y); }
	double getTileBrightness(std::int32_t x, std::int32_t y) const { return map.getTileBrightness(x, y); }

private:
	void resetTickTimer();

	bool updateDraw(double deltaTime);

	std::uint64_t currentTick{};
	std::uint64_t lastUpdateTime{};
	double speedMultiplier{1.0};

	std::uint64_t framesDrawn{};
	std::uint64_t lastFPSLogTime{};

	State state{};

	Map map;
};

export inline Game game;