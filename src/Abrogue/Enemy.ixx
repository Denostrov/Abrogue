export module Enemy;

export import PhysicsComponent;
export import Weapon;

export class Enemy : public PhysicsComponent
{
public:
	Enemy() = default;
	Enemy(EnemyData const& data);

	void update();
	void updateDraw();

	QuadPool::Reference quadReference;
private:
	Weapon weapon;

	Color color;
};