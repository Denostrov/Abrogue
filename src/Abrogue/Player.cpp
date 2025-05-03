module Player;

import GameSystems;

Player::Player(double velocity):PhysicsComponent(36.0, 18.0, 0.48, 0.48, 0.32, 0.4)
{
	quadReference = quadPool.insert(QuadData{{0.0f, 0.0f}, {Helpers::packColor(64, 255, 0, 255), Helpers::packColor(64, 255, 0, 0)}, 64},
									QuadPool::eEntity);

	setMaxVelocity(velocity);
	setHealth(100);

	weapon.init(Weapon::Type::eDagger, 1, 0.25, true);
}

void Player::onMousePressed(std::uint32_t x, std::uint32_t y)
{
	auto [positionX, positionY] = getPosition();
	weapon.startAttack(positionX, positionY, x + 0.5, y + 0.5);
}

void Player::update()
{
	PhysicsComponent::update();

	auto [positionX, positionY] = getPosition();
	weapon.update(positionX, positionY);
}

void Player::updateDraw(double deltaTime)
{
	float guiOffset = 48.0f;

	auto [x, y] = getPosition();
	auto [vx, vy] = getVelocity();
	double drawPositionX = guiOffset + x + vx * deltaTime;
	double drawPositionY = y + vy * deltaTime;
	quadReference.setPosition({drawPositionX * QuadData::tileScale.x, drawPositionY * QuadData::tileScale.y});

	weapon.updateDraw(drawPositionX, drawPositionY);
}

void Player::takeDamage(std::int64_t damage)
{
	std::int64_t newHealth = health - damage;
	if(newHealth < 0)
		newHealth = 0;

	setHealth(newHealth);
}

void Player::setMovement(std::int32_t movementX, std::int32_t movementY)
{
	if(health == 0)
		return;

	setMovementX(movementX);
	setMovementY(movementY);
}

void Player::setHealth(std::int64_t newHealth)
{
	health = newHealth;

	gui.setPlayerHealth(health / 100.0);
	if(health == 0)
	{
		gui.triggerGameOver();
		setMovementX(0);
		setMovementY(0);
	}
}
