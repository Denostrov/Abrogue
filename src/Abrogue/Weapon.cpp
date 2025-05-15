module Weapon;

import Game;
import Player;
import Enemy;

void Weapon::init(WeaponType newType, Color newColor, std::int64_t newDamage, double newAttackTime, bool friendly)
{
	type = newType;
	color = newColor;
	damage = newDamage;
	attackTime = newAttackTime;
	isFriendly = friendly;

	if(type == WeaponType::eClaw)
		drawOffset = 0.2;
	else if(type == WeaponType::eClub)
		drawOffset = 0.2;
	else if(type == WeaponType::eDagger)
		drawOffset = 0.4;
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
			auto quadData = QuadData{{Constants::mapOffset + weaponX, weaponY}, {Color::pack(255, 0, 0, 255), Color::pack(255, 0, 0, 0)}, 249};
			damageReference = quadPool.insert(quadData, QuadPool::ePopup);
		}

		if(isFriendly)
		{
			enemyHandler.inflictDamage(weaponX, weaponY);
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
	weaponReference.setPosition(positionX + weaponOffset * attackAngleCos, positionY + weaponOffset * attackAngleSin * 0.5);
}

void Weapon::startAttack(double positionX, double positionY, double targetPositionX, double targetPositionY)
{
	attackTimer = attackTime;
	auto weaponGlyph = type == WeaponType::eDagger ? 24u : type == WeaponType::eClub ? 20u : 94u;
	QuadData weaponData{{positionX, positionY}, {color.getPacked(), color.getTransparentPacked()}, weaponGlyph};

	double distanceX = (targetPositionX - positionX);
	double distanceY = (targetPositionY - positionY) * 2.0;
	double distance = std::sqrt(distanceX * distanceX + distanceY * distanceY);
	attackAngleCos = distanceX / distance;
	attackAngleSin = distanceY / distance;
	weaponData.setRotation(-attackAngleSin, attackAngleCos);

	weaponReference = quadPool.insert(weaponData, QuadPool::eItem);
}
