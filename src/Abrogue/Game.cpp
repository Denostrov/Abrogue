module;

#include <SDL3/SDL_timer.h>

module Game;

bool Game::init()
{
	QuadPool::init();

	if(!Logger::init())
		return false;

	if(!Configuration::init())
		return false;

	renderEngine = std::make_unique<RenderEngine>();
	if(renderEngine->getHasError())
		return false;

	lastUpdateTime = SDL_GetTicksNS();
	lastFPSLogTime = lastUpdateTime;

	map = Map(Constants::mapWidth, Constants::mapHeight);
	player = Player(10.0);

	initDraw();

	return true;
}

void Game::release()
{
	renderEngine.reset();

	QuadPool::release();
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
			enemies.emplace_back(48 + (double)std::random_device()() / std::numeric_limits<std::uint32_t>::max() * 80.0);
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
		std::uint32_t fps = framesDrawn / (timeSinceLastLog / 1.e9);
		std::array<char, 16> fpsString{"FPS:"};
		std::to_chars(fpsString.data() + 4, fpsString.data() + 14, fps);
		fpsLabel.setText(fpsString.data());

		framesDrawn = 0;
		lastFPSLogTime = currentTime;
	}

	return true;
}

void Game::initDraw()
{
	fpsLabel = Label("FPS:", 0, 0);
	healthLabel = Label("Health", 0, 1);
	hungerLabel = Label("Nutrition", 7, 1);

	float guiOffset = 48.0f;

	auto [x, y] = player.getPosition();
	player.quadReference.setPosition({(guiOffset + x) * QuadData::tileScale.x, y * QuadData::tileScale.y});

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
	float guiOffset = 48.0f;

	auto [x, y] = player.getPosition();
	auto [vx, vy] = player.getVelocity();
	player.quadReference.setPosition({(guiOffset + x + vx * deltaTime) * QuadData::tileScale.x, (y + vy * deltaTime) * QuadData::tileScale.y});

	for(auto const& enemy : enemies)
	{
		auto [x, y] = enemy.getPosition();
		auto [vx, vy] = enemy.getVelocity();
		enemy.quadReference.setPosition({(guiOffset + x + vx * deltaTime) * QuadData::tileScale.x, (y + vy * deltaTime) * QuadData::tileScale.y});
	}

	return renderEngine->drawFrame();
}