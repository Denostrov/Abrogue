module Weapon;

import GameSystems;

void Weapon::init(Type newType, bool friendly)
{
	isFriendly = friendly;
	type = newType;

	switch(type)
	{
		case Type::eClaw:
		{
			damage = 1;
			attackTime = 0.25;
			break;
		}
		case Type::eDagger:
		{
			damage = 1;
			attackTime = 0.25;
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

		if(isFriendly)
		{
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
	double weaponOffset = (attackPeak - std::abs(attackTimer - attackPeak)) / attackPeak + 0.5;
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
