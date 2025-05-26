export module Enemy;

export import PhysicsComponent;
export import Weapon;

export class Enemy : public PhysicsComponent
{
	enum class State
	{
		eSleeping,
		eWandering,
		eHunting
	};

public:
	Enemy() = default;
	Enemy(EnemyData const& data, double positionX, double positionY);

	void update(double playerX, double playerY, std::int64_t stealthRange);
	void updateDraw(double deltaTime);

private:
	State state;
	Weapon weapon;

	Color color;

	double stealthTimer{};

	QuadPool::Reference quad;
};

export class EnemyHandler
{
public:
	EnemyHandler() = default;

	void update();
	void updateDraw(double deltaTime);

	void inflictDamage(double damageX, double damageY);
	void populateLevel();

private:
	double currentTime{};

	FixedVector<Enemy, 4096> enemies;
	double lastEnemySpawnTime{};
};

export inline EnemyHandler enemyHandler;
