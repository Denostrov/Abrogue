module;

#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_scancode.h>

export module Game;

export import RenderEngine;
export import Constants;

class Player
{
public:
	Player()
	{
		QuadPool::insert(QuadData{{x, y}, {0.02f, 0.04f}});
	}

	void setMovementX(int direction) { movementDirectionX = direction; }
	void setMovementY(int direction) { movementDirectionY = direction; }

	void update()
	{
		auto calculateAcceleration = [](double oldVelocity, double force, double mass, double friction, double resistance)
		{
			double result = force / mass * friction - oldVelocity * resistance * friction / mass;
			return result;
		};

		double forceX = movementDirectionX * 15.0 * (movementDirectionY != 0 ? 1.0 / std::sqrt(2.0) : 1.0);
		double forceY = movementDirectionY * 15.0 * (movementDirectionX != 0 ? 1.0 / std::sqrt(2.0) : 1.0);

		velocityX += calculateAcceleration(velocityX, forceX, mass, 1.0, 20.0) * Constants::tickDuration;
		if(std::abs(velocityX) < 1e-6) 
			velocityX = 0.0;

		velocityY += calculateAcceleration(velocityY, forceY, mass, 1.0, 20.0) * Constants::tickDuration;
		if(std::abs(velocityY) < 1e-6)
			velocityY = 0.0;

		x += velocityX * Constants::tickDuration;
		y += velocityY * Constants::tickDuration;

		QuadPool::setData(0, QuadData{{x, y}, {0.02f, 0.04f}});
	}

private:
	double x{}, y{};
	double velocityX{}, velocityY{};
	double mass{1.0};

	int movementDirectionX{}, movementDirectionY{};
};

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

private:
	inline static std::unique_ptr<RenderEngine> renderEngine;

	inline static uint64_t lastUpdateTime{};

	inline static uint64_t framesDrawn{};
	inline static uint64_t lastFPSLogTime{};

	inline static Player player;
	inline static std::array<bool, SDL_Scancode::SDL_SCANCODE_COUNT> pressedButtons{};
};