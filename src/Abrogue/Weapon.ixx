export module Weapon;

export import Configuration;
export import QuadPool;

export class Weapon
{
public:
	Weapon() = default;
	void init(WeaponType newType, std::int64_t newDamage, double newAttackTime, bool friendly);

	bool getIsAttacking() const { return attackTimer > 0.0; }

	void update(double positionX, double positionY);
	void updateDraw(double positionX, double positionY);

	void startAttack(double positionX, double positionY, double targetPositionX, double targetPositionY);

	static void setDrawDebug(bool draw) { drawDebugInfo = draw; }

private:
	inline static bool drawDebugInfo{};

	WeaponType type;
	std::uint64_t damage{};
	double attackTime{};
	double drawOffset{};
	bool isFriendly{};

	QuadPool::Reference weaponReference;
	double attackTimer{};
	double attackAngleCos{};
	double attackAngleSin{};

	QuadPool::Reference damageReference;
};
