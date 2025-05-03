module Weapon;

import GameSystems;

void Weapon::init(Type newType, std::int64_t newDamage, double newAttackTime, bool friendly)
{
	isFriendly = friendly;
	type = newType;
	damage = newDamage;
	attackTime = newAttackTime;

	switch(type)
	{
		case Type::eClaw:
		{
			drawOffset = 0.2;
			break;
		}
		case Type::eDagger:
		{
			drawOffset = 0.4;
			break;
		}
		default:
			break;
	}
}

void Weapon::update(double positionX, double positionY)
{
	if(attackTimer <= 0.0)
		return;

	bool readyToDamage = attackTimer > attackTime / 2.0;
	attackTimer -= Constants::tickDuration;
	if(readyToDamage && attackTimer <= attackTime / 2.0)
	{
		double weaponX = positionX + 1.5 * attackAngleCos;
		double weaponY = positionY + 1.5 * attackAngleSin / 2.0;

		if(drawDebugInfo)
		{
			auto quadData = QuadData{{(48.0 + weaponX) * QuadData::tileScale.x, weaponY * QuadData::tileScale.y}, {Helpers::packColor(255, 0, 0, 255), Helpers::packColor(255, 0, 0, 0)}, 249};
			damageReference = quadPool.insert(quadData, QuadPool::ePopup);
		}

		if(isFriendly)
		{
			auto& enemies = game.getEnemies();
			for(size_t i = 0; i < enemies.size(); i++)
			{
				auto [enemyX, enemyY] = enemies[i].getPosition();
				if(weaponX > enemyX - 0.6 && weaponX < enemyX + 0.6 && weaponY > enemyY - 0.6 && weaponY < enemyY + 0.6)
				{
					enemies.erase(enemies.begin() + i);
					i--;
				}
			}
		}
		else
		{
			auto [playerX, playerY] = player.getPosition();
			if(weaponX > playerX - 0.5 && weaponX < playerX + 0.5 && weaponY > playerY - 0.5 && weaponY < playerY + 0.5)
				player.takeDamage(damage);
		}
	}
	else if(drawDebugInfo)
	{
		damageReference = QuadPool::Reference{};
	}

	if(attackTimer <= 0.0)
	{
		attackTimer = 0.0;
		weaponReference = QuadPool::Reference{};
	}
}

void Weapon::updateDraw(double positionX, double positionY)
{
	if(attackTimer <= 0.0)
		return;

	double attackPeak = attackTime / 2.0;
	double weaponOffset = (attackPeak - std::abs(attackTimer - attackPeak)) / attackPeak + drawOffset;
	weaponReference.setPosition({(positionX + weaponOffset * attackAngleCos) * QuadData::tileScale.x, (positionY + weaponOffset * attackAngleSin * 0.5) * QuadData::tileScale.y});
}

void Weapon::startAttack(double positionX, double positionY, double targetPositionX, double targetPositionY)
{
	attackTimer = attackTime;
	QuadData weaponData{{positionX, positionY}, {Helpers::packColor(255, 255, 0, 255), Helpers::packColor(255, 255, 0, 0)}, type == Type::eDagger ? 24u : 94u};

	double distanceX = (targetPositionX - positionX);
	double distanceY = (targetPositionY - positionY) * 2.0;
	double distance = std::sqrt(distanceX * distanceX + distanceY * distanceY);
	attackAngleCos = distanceX / distance;
	attackAngleSin = distanceY / distance;
	weaponData.setRotation(-attackAngleSin, attackAngleCos);

	weaponReference = quadPool.insert(weaponData, QuadPool::eItem);
}
