module Enemy;

import Player;
import Map;
import Random;

Enemy::Enemy(EnemyData const& data, double positionX, double positionY)
	:PhysicsComponent(positionX, positionY, 0.45, 0.45, 0.45, 0.45), color(data.color)
{
	auto [x, y] = getPosition();
	quad = quadPool.insert(QuadData{{Constants::mapOffset + x, y},
									{color.getPacked(), color.getTransparentPacked()}, data.symbol},
						   QuadPool::eEntity);

	setMass(data.mass);
	setMaxVelocity(data.speed);

	weapon.init(data.weaponType, data.weaponColor, data.damage, data.attackTime, false);
}

void Enemy::update(double playerX, double playerY, double playerVelocityX, double playerVelocityY, std::int64_t stealthRange)
{
	auto [x, y] = getPosition();

	double distanceX = (playerX - x) / 2.0;
	double distanceY = playerY - y;
	double totalDistance = std::sqrt(distanceX * distanceX + distanceY * distanceY);

	auto moveTowards = [this, x, y](double targetX, double targetY)
	{
		double distanceX = (targetX - x) / 2.0;
		double distanceY = targetY - y;
		if(std::abs(distanceX) > std::abs(distanceY))
			setMovementDirection(std::abs(distanceX) < 0.05 ? 0.0 : std::copysign(1.0, distanceX), std::abs(distanceY) < 0.05 ? 0.0 : std::copysign(0.5, distanceY));
		else
			setMovementDirection(std::abs(distanceX) < 0.05 ? 0.0 : std::copysign(0.5, distanceX), std::abs(distanceY) < 0.05 ? 0.0 : std::copysign(1.0, distanceY));
	};

	auto moveWithinDistance = [this, x, y](double targetX, double targetY, double distance)
	{
		double distanceX = (targetX - x) / 2.0;
		double distanceY = targetY - y;
		double totalDistance = std::sqrt(distanceX * distanceX + distanceY * distanceY);

		if(totalDistance > distance + 0.25)
		{
			if(std::abs(distanceX) > std::abs(distanceY))
				setMovementDirection(std::abs(distanceX) < 0.05 ? 0.0 : std::copysign(1.0, distanceX), std::abs(distanceY) < 0.05 ? 0.0 : std::copysign(0.5, distanceY));
			else
				setMovementDirection(std::abs(distanceX) < 0.05 ? 0.0 : std::copysign(0.5, distanceX), std::abs(distanceY) < 0.05 ? 0.0 : std::copysign(1.0, distanceY));
		}
		else if(totalDistance < distance - 0.25)
			setMovementDirection(-distanceX, -distanceY);
	};

	if(state == State::eHunting)
	{
		auto [targetTileX, targetTileY] = path[currentPathIndex];

		if((std::int64_t)x != targetTileX || (std::int64_t)y != targetTileY)
		{
			moveTowards(targetTileX + 0.5, targetTileY + 0.5);
		}
		else
		{
			currentPathIndex++;
			if(currentPathIndex >= path.size())
				state = State::eWandering;
		}
	}
	else if(state == State::eSleeping)
	{
		if(totalDistance < stealthRange && map.getTileInLineOfSight(x, y))
		{
			stealthTimer += Constants::tickDuration;
			if(stealthTimer > lastCheckedStealthTime + 0.5)
			{
				lastCheckedStealthTime += 0.5;
				std::uint64_t detectRoll = mapRandom.generate() % 4;
				if(detectRoll == 0)
				{
					state = State::eHunting;
					stealthTimer = 0.0;
					lastCheckedStealthTime = 0.0;
					path = map.getPath(x, y, playerX, playerY);
					currentPathIndex = 0;
				}
			}
		}
	}
	else if(state == State::eWandering)
	{
		if(totalDistance < stealthRange && map.getTileInLineOfSight(x, y))
		{
			if((std::int64_t)x != (std::int64_t)playerX || (std::int64_t)y != (std::int64_t)playerY)
			{
				state = State::eHunting;
				path = map.getPath(x, y, playerX, playerY);
				currentPathIndex = 0;
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
	quad.setPosition(Constants::mapOffset + x + vx * deltaTime, y + vy * deltaTime);

	auto brightness = map.getTileBrightness(x, y);
	if(brightness < Constants::mapMinBrightness)
	{
		quad.setColor(0);
		quad.setBackgroundColor(0);
	}
	else
	{
		quad.setColor(Color::pack(color.r * brightness, color.g * brightness, color.b * brightness, color.a));
		quad.setBackgroundColor(Color::pack(color.r * brightness, color.g * brightness, color.b * brightness, 0));

		weapon.updateDraw(Constants::mapOffset + x, y);
	}
}

void EnemyHandler::update()
{
	currentTime += Constants::tickDuration;

	if(currentTime - lastEnemySpawnTime > 60.0)
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
	auto [playerVelocityX, playerVelocityY] = player.getVelocity();
	auto stealthRange = player.getStealthRange();
	for(auto& enemy : enemies)
		enemy.update(playerX, playerY, playerVelocityX, playerVelocityY, stealthRange);
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

void EnemyHandler::populateLevel()
{
	for(std::int64_t i = 0; i < 20; i++)
	{
		if(auto enemyDataOpt = configuration.getSuitableEnemy())
		{
			auto const& spawnRoom = map.getRandomRoom();
			std::int64_t spawnX = spawnRoom.originX + mapRandom.generate() % spawnRoom.width;
			std::int64_t spawnY = spawnRoom.originY + mapRandom.generate() % spawnRoom.height;

			if(map.getTileInLineOfSight(spawnX, spawnY))
			{
				i--;
				continue;
			}

			enemies.emplace_back(*enemyDataOpt, spawnX + 0.5, spawnY + 0.5);
		}
	}
}
