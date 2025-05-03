module Enemy;

import GameSystems;

Enemy::Enemy(std::uint8_t type, double speed, double mass, std::int64_t damage, double attackTime)
	:PhysicsComponent(36.0, 18.0, 0.48, 0.48, 0.48, 0.48)
{
	quadReference = quadPool.insert(QuadData{{0.0f, 0.0f}, {Helpers::packColor(255, 0, 0, 255), Helpers::packColor(255, 0, 0, 0)}, type},
									QuadPool::eEntity);

	setMass(mass);
	setMaxVelocity(speed);

	weapon.init(Weapon::Type::eClaw, damage, attackTime, false);
}

void Enemy::update()
{
	auto [playerX, playerY] = player.getPosition();
	auto [x, y] = getPosition();

	setMovementX(playerX > x + 1.0 ? 1 : playerX < x - 1.0 ? -1 : 0);
	setMovementY(playerY > y + 1.0 ? 1 : playerY < y - 1.0 ? -1 : 0);

	if(std::abs(playerX - x) < 2.0 && std::abs(playerY - y) < 1.0 && !weapon.getIsAttacking())
		weapon.startAttack(x, y, playerX, playerY);

	PhysicsComponent::update();

	std::tie(x, y) = getPosition();
	weapon.update(x, y);
}

void Enemy::updateDraw()
{
	auto [x, y] = getPosition();
	weapon.updateDraw(48.0 + x, y);
}
