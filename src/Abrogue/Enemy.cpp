module Enemy;

import GameSystems;

Enemy::Enemy(std::uint8_t type):PhysicsComponent(36.0, 18.0, 0.48, 0.48, 0.48, 0.48)
{
	auto [x, y] = getPosition();
	quadReference = quadPool.insert(QuadData{{0.0f, 0.0f}, {Helpers::packColor(255, 0, 0, 255), Helpers::packColor(255, 0, 0, 0)}, type},
									QuadPool::eEntity);

	setMass(10.0 + (double)std::random_device()() / std::numeric_limits<std::uint32_t>::max() * 10.0);
	setFrictionCoefficient((double)std::random_device()() / std::numeric_limits<std::uint32_t>::max());
	setMaxVelocity(1.0 + (double)std::random_device()() / std::numeric_limits<std::uint32_t>::max() * 9.0);
}

void Enemy::update()
{
	auto [playerX, playerY] = game.getPlayerPosition();
	auto [x, y] = getPosition();

	setMovementX(playerX > x ? 1 : -1);
	setMovementY(playerY > y ? 1 : -1);

	PhysicsComponent::update();
}