module Player;

import GameSystems;

Player::Player(double velocity) :PhysicsComponent(36.0, 18.0, 0.48, 0.48, 0.32, 0.4)
{
	quadReference = quadPool.insert(QuadData{{0.0f, 0.0f}, {Helpers::packColor(64, 255, 0, 255), Helpers::packColor(64, 255, 0, 0)}, 64},
									QuadPool::eEntity);

	setMaxVelocity(velocity);
	weapon.damage = 1;
}

void Player::onMousePressed(std::uint32_t x, std::uint32_t y)
{
	attackTimer = 1.0;
	auto [positionX, positionY] = getPosition();
	weaponReference = quadPool.insert(QuadData{{positionX - 1.0f, positionY}, {Helpers::packColor(255, 255, 0, 255), Helpers::packColor(255, 255, 0, 0)}, 24}, QuadPool::eItem);
}
