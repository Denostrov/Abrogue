export module Enemy;

export import PhysicsComponent;
export import Weapon;

export class Enemy : public PhysicsComponent
{
public:
	Enemy() = default;
	Enemy(std::uint8_t type, double speed, double mass, WeaponType weaponType, std::int64_t damage, double attackTime);

	void update();
	void updateDraw();

	QuadPool::Reference quadReference;
private:
	Weapon weapon;
};