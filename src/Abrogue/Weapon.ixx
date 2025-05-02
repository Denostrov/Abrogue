export module Weapon;

export import std;
export import QuadPool;

export class Weapon
{
public:
	enum class Type
	{
		eClaw,
		eDagger
	};

	Weapon() = default;
	void init(Type newType, bool friendly);

	bool getIsAttacking() const { return attackTimer > 0.0; }

	void update(double positionX, double positionY);
	void updateDraw(double positionX, double positionY);

	void startAttack(double positionX, double positionY, double targetPositionX, double targetPositionY);

private:
	Type type;
	std::uint64_t damage{};
	double attackTime{};
	bool isFriendly{};

	QuadPool::Reference weaponReference;
	double attackTimer{};
	double attackAngleCos{};
	double attackAngleSin{};
};
