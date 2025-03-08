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

	static bool init();
	static void release();
	static bool update();

	static void onKeyPressed(SDL_Scancode scanCode) { pressedButtons[scanCode] = true; }
	static void onKeyReleased(SDL_Scancode scanCode) { pressedButtons[scanCode] = false; }

	static std::pair<double, double> getPlayerPosition() { return player.getPosition(); }
	static bool getTileSolid(std::uint32_t x, std::uint32_t y) { return map.getTileExists(x, y); }

private:
	static void initDraw();
	static bool updateDraw(double deltaTime);

	inline static std::unique_ptr<RenderEngine> renderEngine;

	inline static uint64_t lastUpdateTime{};

	inline static uint64_t framesDrawn{};
	inline static uint64_t lastFPSLogTime{};

	inline static State state{};

	inline static Map map;
	inline static Player player;
	inline static std::vector<Enemy> enemies;

	inline static GUI gui;

	inline static std::array<bool, SDL_Scancode::SDL_SCANCODE_COUNT> pressedButtons{};
};