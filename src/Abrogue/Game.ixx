export module Game;

export import RenderEngine;
export import Player;
export import Enemy;
export import Map;
export import GUI;

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

	void startGame();
	void quitToDesktop();

	void setPaused(bool paused);

	void quitToMainMenu();

	void onMouseMoved(float x, float y);
	void onMousePressed(float x, float y);

	auto getState() const { return state; }
	bool getTileSolid(std::uint32_t x, std::uint32_t y) { return map.getTileExists(x, y); }

private:
	void resetTickTimer();

	void initDraw();
	bool updateDraw(double deltaTime);

	RenderEngine renderEngine;

	uint64_t currentTick{};
	uint64_t lastUpdateTime{};

	uint64_t framesDrawn{};
	uint64_t lastFPSLogTime{};

	State state{};

	Map map;
	std::vector<Enemy> enemies;
};