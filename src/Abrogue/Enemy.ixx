export module Enemy;

export import PhysicsComponent;
export import Weapon;

export class Enemy : public PhysicsComponent
{
public:
	Enemy() = default;
	Enemy(EnemyData const& data, double positionX, double positionY);

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

	void update();
	void updateDraw(double deltaTime);

	void inflictDamage(double damageX, double damageY);

private:
	double currentTime{};

	FixedVector<Enemy, 4096> enemies;
	double lastEnemySpawnTime{};
};

export inline EnemyHandler enemyHandler;
