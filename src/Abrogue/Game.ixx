module;

#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_scancode.h>

export module Game;

export import RenderEngine;
export import Player;
export import Enemy;

export class Game
{
public:
	static bool init()
	{
		renderEngine = std::make_unique<RenderEngine>();
		if(renderEngine->getHasError())
			return false;

		lastUpdateTime = SDL_GetTicksNS();
		lastFPSLogTime = lastUpdateTime;
		return true;
	}
	static void release()
	{
		renderEngine.reset();
	}

	static bool update()
	{
		player.setMovementX(pressedButtons[SDL_SCANCODE_D] - pressedButtons[SDL_SCANCODE_A]);
		player.setMovementY(pressedButtons[SDL_SCANCODE_S] - pressedButtons[SDL_SCANCODE_W]);

		uint64_t currentTime = SDL_GetTicksNS();
		uint64_t updateCount{};
		while(currentTime - lastUpdateTime > Constants::tickDurationNS)
		{
			player.update();

			if(lastUpdateTime / 5000000000 > enemies.size())
				enemies.emplace_back();
			for(auto& enemy : enemies) enemy.update();

			lastUpdateTime += Constants::tickDurationNS;

			updateCount++;
			if(updateCount > 4)
			{
				lastUpdateTime = currentTime;
				Logger::logInfo("Can't keep up, skipping ticks");
				break;
			}
		}

		if(!renderEngine->drawFrame())
			return false;

		framesDrawn++;
		uint64_t timeSinceLastLog = currentTime - lastFPSLogTime;
		if(timeSinceLastLog > 1000000000)
		{
			Logger::logInfo(std::format("FPS: {}", framesDrawn / (timeSinceLastLog / 1.e9)));
			framesDrawn = 0;
			lastFPSLogTime = currentTime;
		}

		return true;
	}

	static void onKeyPressed(SDL_Scancode scanCode)
	{
		pressedButtons[scanCode] = true;
	}
	static void onKeyReleased(SDL_Scancode scanCode)
	{
		pressedButtons[scanCode] = false;
	}

	static std::pair<double, double> getPlayerPosition() { return player.getPosition(); }

private:
	inline static std::unique_ptr<RenderEngine> renderEngine;

	inline static uint64_t lastUpdateTime{};

	inline static uint64_t framesDrawn{};
	inline static uint64_t lastFPSLogTime{};

	inline static Player player;
	inline static std::vector<Enemy> enemies;

	inline static std::array<bool, SDL_Scancode::SDL_SCANCODE_COUNT> pressedButtons{};
};