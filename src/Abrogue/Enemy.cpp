module Enemy;

import Player;
import Map;
import Random;

Enemy::Enemy(EnemyData const& data, double positionX, double positionY)
	:PhysicsComponent(positionX, positionY, 0.45, 0.45, 0.45, 0.45), color(data.color)
{
	auto [x, y] = getPosition();
	quadReference = quadPool.insert(QuadData{{Constants::mapOffset + x, y},
									{color.getPacked(), color.getTransparentPacked()}, data.symbol},
									QuadPool::eEntity);

	setMass(data.mass);
	setMaxVelocity(data.speed);

	weapon.init(data.weaponType, data.weaponColor, data.damage, data.attackTime, false);
}

void Enemy::update(double playerX, double playerY, std::int64_t stealthRange)
{
	auto [x, y] = getPosition();

	double distanceX = playerX - x;
	double distanceY = playerY - y;
	double totalDistance = std::sqrt(distanceX * distanceX / 4.0 + distanceY * distanceY);

	if(state == State::eHunting)
	{
		if(totalDistance > stealthRange || map.getTileBrightness(x, y) <= 0.0)
			state = State::eWandering;

		if(totalDistance > 1.5)
		{
			setMovementX(x < playerX - 0.5 ? 1 : x > playerX + 0.5 ? -1 : 0);
			setMovementY(y < playerY - 0.5 ? 1 : y > playerY + 0.5 ? -1 : 0);
		}
		else if(totalDistance < 1.0)
		{
			setMovementX(x < playerX ? -1 : x >= playerX ? 1 : 0);
			setMovementY(y < playerY ? -1 : y >= playerY ? 1 : 0);
		}

		if(totalDistance < 1.5 && !weapon.getIsAttacking())
			weapon.startAttack(x, y, playerX, playerY);
	}
	else
	{
		if(totalDistance < stealthRange && map.getTileBrightness(x, y) > 0.0)
		{
			std::int64_t timerWhole = stealthTimer;
			stealthTimer += Constants::tickDuration;
			if((std::int64_t)stealthTimer > timerWhole)
			{
				std::uint64_t detectRoll = mapRandom.generate() % 2;
				if(detectRoll)
					state = State::eHunting;
			}
		}
	}

	PhysicsComponent::update();

	std::tie(x, y) = getPosition();
	weapon.update(x, y);
}

void Enemy::updateDraw(double deltaTime)
{
	auto [x, y] = getPosition();
	auto [vx, vy] = getVelocity();
	quadReference.setPosition(Constants::mapOffset + x + vx * deltaTime, y + vy * deltaTime);

	auto brightness = map.getTileBrightness(x, y);
	if(brightness <= 0.0)
	{
		quadReference.setColor(0);
		quadReference.setBackgroundColor(0);
	}
	else
	{
		quadReference.setColor(Color::pack(color.r * brightness, color.g * brightness, color.b * brightness, color.a));
		quadReference.setBackgroundColor(Color::pack(color.r * brightness, color.g * brightness, color.b * brightness, 0));

		weapon.updateDraw(Constants::mapOffset + x, y);
	}
}

void EnemyHandler::update()
{
	currentTime += Constants::tickDuration;

	if(currentTime - lastEnemySpawnTime > 10.0)
	{
		if(auto enemyDataOpt = configuration.getSuitableEnemy())
		{
			auto const& spawnRoom = map.getRandomRoom();
			std::int64_t spawnX = spawnRoom.originX + mapRandom.generate() % spawnRoom.width;
			std::int64_t spawnY = spawnRoom.originY + mapRandom.generate() % spawnRoom.height;

			enemies.emplace_back(*enemyDataOpt, spawnX + 0.5, spawnY + 0.5);
		}

		lastEnemySpawnTime = currentTime;
	}

	auto [playerX, playerY] = player.getPosition();
	auto stealthRange = player.getStealthRange();
	for(auto& enemy : enemies)
		enemy.update(playerX, playerY, stealthRange);
}

void EnemyHandler::updateDraw(double deltaTime)
{
	for(auto& enemy : enemies)
		enemy.updateDraw(deltaTime);
}

void EnemyHandler::inflictDamage(double damageX, double damageY)
{
	for(size_t i = 0; i < enemies.size(); i++)
	{
		auto [enemyX, enemyY] = enemies[i].getPosition();
		if(damageX > enemyX - 0.6 && damageX < enemyX + 0.6 && damageY > enemyY - 0.6 && damageY < enemyY + 0.6)
		{
			enemies.erase(enemies.begin() + i);
			i--;
		}
	}
}
