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
	void quitToDesktop();

	void setPaused(bool paused);
	void setSpeedMultiplier(double speed);

	void quitToMainMenu();

	auto getState() const { return state; }
	auto& getEnemies() { return enemies; }
	bool getTileSolid(std::uint32_t x, std::uint32_t y) { return map.getTileExists(x, y); }

private:
	void resetTickTimer();

	void initDraw();
	bool updateDraw(double deltaTime);

	std::uint64_t currentTick{};
	std::uint64_t lastUpdateTime{};
	double speedMultiplier{1.0};

	std::uint64_t framesDrawn{};
	std::uint64_t lastFPSLogTime{};

	State state{};

	Map map;
	std::vector<Enemy> enemies;
	double lastEnemySpawnTime{};
};