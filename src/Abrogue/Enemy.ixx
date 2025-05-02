export module Enemy;

export import PhysicsComponent;
export import Weapon;

export class Enemy : public PhysicsComponent
{
public:
	Enemy() = default;
	Enemy(std::uint8_t type, double speed, double mass);

	void update();
	void updateDraw();

	QuadPool::Reference quadReference;
private:
	Weapon weapon;
};