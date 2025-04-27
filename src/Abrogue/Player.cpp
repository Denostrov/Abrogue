module Player;

import GameSystems;

Player::Player(double velocity) :PhysicsComponent(36.0, 18.0, 0.48, 0.48, 0.32, 0.4)
{
	quadReference = quadPool.insert(QuadData{{0.0f, 0.0f}, {Helpers::packColor(64, 255, 0, 255), Helpers::packColor(64, 255, 0, 0)}, 64},
									QuadPool::eEntity);

	setMaxVelocity(velocity);
	weapon.damage = 1;
	weapon.attackTime = 0.25;
}

void Player::onMousePressed(std::uint32_t x, std::uint32_t y)
{
	attackTimer = weapon.attackTime;
	auto [positionX, positionY] = getPosition();
	weaponReference = quadPool.insert(QuadData{{positionX, positionY}, {Helpers::packColor(255, 255, 0, 255), Helpers::packColor(255, 255, 0, 0)}, 24}, QuadPool::eItem);

	double distanceX = (x + 0.5 - positionX);
	double distanceY = (y + 0.5 - positionY);
	double distance = std::sqrt(distanceX * distanceX + distanceY * distanceY);
	attackAngleCos = distanceX / distance;
	attackAngleSin = distanceY / distance;
}
