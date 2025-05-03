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

	lastUpdateTime = SDL_GetTicksNS();
	lastFPSLogTime = lastUpdateTime;

	gui.init();

	resetTickTimer();

	return true;
}

bool Game::update()
{
	player.setMovementX(inputHandler.getButtonPressed(SDL_SCANCODE_D) - inputHandler.getButtonPressed(SDL_SCANCODE_A));
	player.setMovementY(inputHandler.getButtonPressed(SDL_SCANCODE_S) - inputHandler.getButtonPressed(SDL_SCANCODE_W));

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
			enemies.emplace_back(enemyData.symbol, enemyData.speed, enemyData.mass);
			lastEnemySpawnTime = currentTick / (double)Constants::ticksPerSecond;
		}

		for(auto& enemy : enemies)
			enemy.update();
	}
}

void Game::startGame()
{
	map = Map(Constants::mapWidth, Constants::mapHeight);
	player = Player(10.0);

	initDraw();

	state = eRunning;

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

void Game::onMouseMoved(float x, float y)
{
	auto [width, height] = renderEngine.getFramebufferSize();
	gui.onMouseMoved(x / width * Constants::screenWidth, y / height * Constants::screenHeight);
}

void Game::onMousePressed(float x, float y)
{
	auto [width, height] = renderEngine.getFramebufferSize();
	gui.onMousePressed(x / width * Constants::screenWidth, y / height * Constants::screenHeight);
}

void Game::resetTickTimer()
{
	lastUpdateTime = SDL_GetTicksNS();
	currentTick = 0;
}

void Game::initDraw()
{
	gui.startGame();

	float guiOffset = 48.0f;

	for(auto const& enemy : enemies)
	{
		auto [x, y] = enemy.getPosition();
		enemy.quadReference.setPosition({(guiOffset + x) * QuadData::tileScale.x, y * QuadData::tileScale.y});
	}

	auto const& mapTiles = map.getTiles();
	for(size_t i = 0; i < mapTiles.size(); i++)
	{
		size_t row = i / Constants::mapWidth;
		size_t column = i % Constants::mapWidth;
		mapTiles[i].quadReference.setPosition({(guiOffset + column + 0.5f) * QuadData::tileScale.x,
											  (row + 0.5f) * QuadData::tileScale.y});
	}
}

bool Game::updateDraw(double deltaTime)
{
	if(state == eRunning)
	{
		float guiOffset = 48.0f;

		player.updateDraw(deltaTime);

		for(auto& enemy : enemies)
		{
			auto [x, y] = enemy.getPosition();
			auto [vx, vy] = enemy.getVelocity();
			enemy.quadReference.setPosition({(guiOffset + x + vx * deltaTime) * QuadData::tileScale.x, (y + vy * deltaTime) * QuadData::tileScale.y});
			enemy.updateDraw();
		}
	}

	return renderEngine.drawFrame();
}