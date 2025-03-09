module;

#include <SDL3/SDL_scancode.h>

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
		ePaused
	};

	bool init();
	bool update();

	void startGame();

	void onKeyPressed(SDL_Scancode scanCode) { pressedButtons[scanCode] = true; }
	void onKeyReleased(SDL_Scancode scanCode) { pressedButtons[scanCode] = false; }

	void onMousePressed(float x, float y);

	std::pair<double, double> getPlayerPosition() { return player.getPosition(); }
	bool getTileSolid(std::uint32_t x, std::uint32_t y) { return map.getTileExists(x, y); }

private:
	void initDraw();
	bool updateDraw(double deltaTime);

	RenderEngine renderEngine;

	uint64_t lastUpdateTime{};

	uint64_t framesDrawn{};
	uint64_t lastFPSLogTime{};

	State state{};

	Map map;
	Player player;
	std::vector<Enemy> enemies;

	GUI gui;

	std::array<bool, SDL_Scancode::SDL_SCANCODE_COUNT> pressedButtons{};
};