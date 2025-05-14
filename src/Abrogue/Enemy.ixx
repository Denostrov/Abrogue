export module Enemy;

export import PhysicsComponent;
export import Weapon;

export class Enemy : public PhysicsComponent
{
public:
	Enemy() = default;
	Enemy(EnemyData const& data);

	void update();
	void updateDraw(double deltaTime);

	QuadPool::Reference quadReference;
private:
	Weapon weapon;

	Color color;
};

export class EnemyHandler
{
public:
	EnemyHandler() = default;

private:
	FixedVector<Enemy, 4096> enemies;
};

export inline EnemyHandler enemyHandler;