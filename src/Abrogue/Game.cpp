module;

#include <SDL3/SDL_timer.h>

module Game;

bool Game::init()
{
	renderEngine = std::make_unique<RenderEngine>();
	if(renderEngine->getHasError())
		return false;

	lastUpdateTime = SDL_GetTicksNS();
	lastFPSLogTime = lastUpdateTime;

	initDraw();

	return true;
}

void Game::release()
{
	renderEngine.reset();
}

bool Game::update()
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

	if(!updateDraw((currentTime - lastUpdateTime) / 1000000000.0))
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

void Game::initDraw()
{
	float guiOffset = QuadData::tileScale.x * 48.0f;

	auto [x, y] = player.getPosition();
	player.quadReference.setPosition({guiOffset + x, y});

	for(auto const& enemy : enemies)
	{
		auto [x, y] = enemy.getPosition();
		enemy.quadReference.setPosition({guiOffset + x, y});
	}

	auto const& mapTiles = map.getTiles();
	for(size_t i = 0; i < mapTiles.size(); i++)
	{
		size_t row = i / Constants::mapWidth;
		size_t column = i % Constants::mapWidth;
		mapTiles[i].quadReference.setPosition({guiOffset + column * QuadData::tileScale.x + QuadData::tileScale.x / 2.0f,
											  row * QuadData::tileScale.y + QuadData::tileScale.y / 2.0f});
	}
}

bool Game::updateDraw(double deltaTime)
{
	float guiOffset = QuadData::tileScale.x * 48.0f;

	auto [x, y] = player.getPosition();
	auto [vx, vy] = player.getVelocity();
	player.quadReference.setPosition({guiOffset + x + vx * deltaTime, y + vy * deltaTime});

	for(auto const& enemy : enemies)
	{
		auto [x, y] = enemy.getPosition();
		auto [vx, vy] = enemy.getVelocity();
		enemy.quadReference.setPosition({guiOffset + x + vx * deltaTime, y + vy * deltaTime});
	}

	return renderEngine->drawFrame();
}