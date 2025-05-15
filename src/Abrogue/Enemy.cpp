module Enemy;

import Player;
import Game;
import Random;

Enemy::Enemy(EnemyData const& data, double positionX, double positionY)
	:PhysicsComponent(positionX, positionY, 0.48, 0.48, 0.48, 0.48), color(data.color)
{
	auto [x, y] = getPosition();
	quadReference = quadPool.insert(QuadData{{Constants::mapOffset + x, y},
									{color.getPacked(), color.getTransparentPacked()}, data.symbol},
									QuadPool::eEntity);

	setMass(data.mass);
	setMaxVelocity(data.speed);

	weapon.init(data.weaponType, data.weaponColor, data.damage, data.attackTime, false);
}

void Enemy::update()
{
	auto [playerX, playerY] = player.getPosition();
	auto [x, y] = getPosition();

	if(x < playerX - 1.5 || (x > playerX && x < playerX + 1.0))
		setMovementX(1);
	else if(x > playerX + 1.5 || (x < playerX && x > playerX - 1.0))
		setMovementX(-1);
	else
		setMovementX(0);

	if(y < playerY - 1.0 || (y > playerY && y < playerY + 0.5))
		setMovementY(1);
	else if(y > playerY + 1.0 || (y < playerY && y > playerY - 0.5))
		setMovementY(-1);
	else
		setMovementY(0);

	if(std::abs(playerX - x) < 2.0 && std::abs(playerY - y) < 1.0 && !weapon.getIsAttacking())
		weapon.startAttack(x, y, playerX, playerY);

	PhysicsComponent::update();

	std::tie(x, y) = getPosition();
	weapon.update(x, y);
}

void Enemy::updateDraw(double deltaTime)
{
	auto [x, y] = getPosition();
	auto [vx, vy] = getVelocity();
	quadReference.setPosition(Constants::mapOffset + x + vx * deltaTime, y + vy * deltaTime);

	auto brightness = game.getTileBrightness(x, y);
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
			auto const& spawnRoom = game.getRandomRoom();
			std::int64_t spawnX = spawnRoom.originX + mapRandom.generate() % spawnRoom.width;
			std::int64_t spawnY = spawnRoom.originY + mapRandom.generate() % spawnRoom.height;

			enemies.emplace_back(*enemyDataOpt, spawnX + 0.5, spawnY + 0.5);
		}

		lastEnemySpawnTime = currentTime;
	}

	for(auto& enemy : enemies)
		enemy.update();
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
