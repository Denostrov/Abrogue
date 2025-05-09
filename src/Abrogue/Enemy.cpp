module Enemy;

import GameSystems;

Enemy::Enemy(EnemyData const& data)
	:PhysicsComponent(40.5, 33.5, 0.48, 0.48, 0.48, 0.48), color(data.color)
{
	auto [x, y] = getPosition();
	quadReference = quadPool.insert(QuadData{{(48.0f + x) * QuadData::tileScale.x, y * QuadData::tileScale.y},
									{color.getPacked(), color.getTransparentPacked()}, data.symbol},
									QuadPool::eEntity);

	setMass(data.mass);
	setMaxVelocity(data.speed);

	weapon.init(data.weaponType, data.damage, data.attackTime, false);
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
	auto brightness = game.getTileBrightness(x, y);
	if(brightness <= 0.0)
	{
		quadReference.setColor(0);
		quadReference.setBackgroundColor(0);
	}
	else
	{
		quadReference.setColor(Helpers::packColor(color.r * brightness, color.g * brightness, color.b * brightness, color.a));
		quadReference.setBackgroundColor(Helpers::packColor(color.r * brightness, color.g * brightness, color.b * brightness, 0));

		weapon.updateDraw(48.0 + x, y);
	}
}
