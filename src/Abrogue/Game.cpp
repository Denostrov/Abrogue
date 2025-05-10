module;

#include <SDL3/SDL_timer.h>

module Game;

import GameSystems;

bool Game::init()
{
	quadPool.prepare();

	if(!logger.openFiles())
		return false;

	if(!configuration.load())
		return false;

	if(!renderEngine.initVulkan())
		return false;

	gui.init();

	lastUpdateTime = SDL_GetTicksNS();
	lastFPSLogTime = lastUpdateTime;

	resetTickTimer();

	return true;
}

bool Game::update()
{
	uint64_t currentTime = SDL_GetTicksNS();
	uint64_t updateCount{};
	while((currentTime - lastUpdateTime) * speedMultiplier > Constants::tickDurationNS)
	{
		advanceStep();

		lastUpdateTime += Constants::tickDurationNS / speedMultiplier;

		updateCount++;
		if(updateCount > 4)
		{
			lastUpdateTime = currentTime;
			logger.logInfo("Can't keep up, skipping ticks");
			break;
		}
	}

	if(!updateDraw((currentTime - lastUpdateTime) * speedMultiplier / 1000000000.0))
		return false;

	framesDrawn++;
	uint64_t timeSinceLastLog = currentTime - lastFPSLogTime;
	if(timeSinceLastLog > 1000000000)
	{
		std::uint32_t fps = framesDrawn / (timeSinceLastLog / 1.e9);
		gui.setFPS(fps);

		framesDrawn = 0;
		lastFPSLogTime = currentTime;
	}

	return true;
}

void Game::advanceStep()
{
	if(state == eRunning)
	{
		currentTick++;

		player.update();

		if(currentTick / (double)Constants::ticksPerSecond > lastEnemySpawnTime + 3.0)
		{
			auto const& enemyData = configuration.getSuitableEnemy();
			enemies.emplace_back(enemyData);
			lastEnemySpawnTime = currentTick / (double)Constants::ticksPerSecond;
		}

		for(auto& enemy : enemies)
			enemy.update();
	}
}

void Game::startGame()
{
	map.init();
	player = Player(10.0);

	gui.startGame();

	state = eRunning;

	lastUpdateTime = SDL_GetTicksNS();
	resetTickTimer();
}

void Game::quitToDesktop()
{
	state = eFinished;
}

void Game::setPaused(bool paused)
{
	state = paused ? ePaused : eRunning;
}

void Game::setSpeedMultiplier(double speed)
{
	logger.extraAssert(speed >= 0.0, "Set incorrect speed multiplier");

	speedMultiplier = speed;
	lastUpdateTime = SDL_GetTicksNS();
}

void Game::quitToMainMenu()
{
	state = eNotStarted;
	map = Map();
	player = Player();
	enemies.clear();
}

void Game::resetTickTimer()
{
	currentTick = 0;
	lastEnemySpawnTime = 0.0;
}

bool Game::updateDraw(double deltaTime)
{
	gui.updateDraw(deltaTime);

	if(state == eRunning)
	{
		player.updateDraw(deltaTime);

		map.updateDraw(deltaTime);

		for(auto& enemy : enemies)
			enemy.updateDraw(deltaTime);
	}

	return renderEngine.drawFrame();
}