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
	QuadData weaponData{{positionX, positionY}, {Helpers::packColor(255, 255, 0, 255), Helpers::packColor(255, 255, 0, 0)}, 24};

	double distanceX = (x + 0.5 - positionX);
	double distanceY = (y + 0.5 - positionY) * 2.0;
	double distance = std::sqrt(distanceX * distanceX + distanceY * distanceY);
	attackAngleCos = distanceX / distance;
	attackAngleSin = distanceY / distance;
	weaponData.setRotation(-attackAngleSin, attackAngleCos);

	weaponReference = quadPool.insert(weaponData, QuadPool::eItem);
}

void Player::update()
{
	PhysicsComponent::update();

	if(attackTimer > 0.0)
	{
		bool readyToDamage = attackTimer > weapon.attackTime / 2.0;
		attackTimer -= Constants::tickDuration;
		if(readyToDamage && attackTimer <= weapon.attackTime / 2.0)
		{
			auto [positionX, positionY] = getPosition();
			double weaponX = positionX + 1.5 * attackAngleCos;
			double weaponY = positionY + 1.5 * attackAngleSin / 2.0;

			auto& enemies = game.getEnemies();
			for(size_t i = 0; i < enemies.size(); i++)
			{
				auto [enemyX, enemyY] = enemies[i].getPosition();
				if(weaponX > enemyX - 0.5 && weaponX < enemyX + 0.5 && weaponY > enemyY - 0.5 && weaponY < enemyY + 0.5)
				{
					enemies.erase(enemies.begin() + i);
					i--;
				}
			}
		}

		if(attackTimer <= 0.0)
		{
			attackTimer = 0.0;
			weaponReference = QuadPool::Reference{};
		}
	}
}

void Player::updateDraw(double deltaTime)
{
	float guiOffset = 48.0f;

	auto [x, y] = getPosition();
	auto [vx, vy] = getVelocity();
	quadReference.setPosition({(guiOffset + x + vx * deltaTime) * QuadData::tileScale.x, (y + vy * deltaTime) * QuadData::tileScale.y});
	if(attackTimer > 0.0)
	{
		double attackPeak = weapon.attackTime / 2.0;
		double weaponOffset = (attackPeak - std::abs(attackTimer - attackPeak)) / attackPeak + 0.5;
		weaponReference.setPosition({(guiOffset + x + vx * deltaTime + weaponOffset * attackAngleCos) * QuadData::tileScale.x, (y + vy * deltaTime + weaponOffset * attackAngleSin * 0.5) * QuadData::tileScale.y});
	}
}
