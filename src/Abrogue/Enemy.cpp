module Enemy;

import Game;

Enemy::Enemy()
{
	type = 48 + (float)std::random_device()() / std::numeric_limits<std::uint32_t>::max() * 48.0f;
	auto [x, y] = getPosition();
	quadReference = QuadPool::insert(QuadData{{x, y}, {0.01666f, 0.03333f},
									 {QuadData::packColor(255, 255, 255, 255), QuadData::packColor(255, 255, 255, 255), 0}, type});

	setMass(10.0 + (double)std::random_device()() / std::numeric_limits<std::uint32_t>::max() * 10.0);
	setFrictionCoefficient((double)std::random_device()() / std::numeric_limits<std::uint32_t>::max());
	setMaxSpeed(0.5 + (double)std::random_device()() / std::numeric_limits<std::uint32_t>::max());
}

void Enemy::update()
{
	auto [playerX, playerY] = Game::getPlayerPosition();
	auto [x, y] = getPosition();

	setMovementX(playerX > x ? 1 : -1);
	setMovementY(playerY > y ? 1 : -1);

	PhysicsComponent::update();
	std::tie(x, y) = getPosition();
	quadReference.setPosition({x, y});
}