export module Enemy;

export import PhysicsComponent;
export import Weapon;

export class EnemyHandler
{
	class Enemy: public PhysicsComponent
	{
	public:
		enum class State
		{
			eSleeping,
			eWandering,
			eHunting
		};

		Enemy() = default;
		Enemy(EnemyData const& data, double positionX, double positionY, State initialState);

		void update(double playerX, double playerY, double playerVelocityX, double playerVelocityY, std::int64_t stealthRange);
		void updateDraw(double deltaTime);

		void updateDrawDebug();
	private:
		void setState(State newState);
		void setPathTo(std::int64_t x, std::int64_t y);

		State state;
		Weapon weapon;

		Color color;

		double stealthTimer{};
		double lastCheckedStealthTime{};

		FixedVector<std::pair<std::int64_t, std::int64_t>, 128> path;
		std::int64_t currentPathIndex{};

		QuadPool::Reference quad;

		QuadPool::Reference stateQuad;
		FixedVector<QuadPool::Reference, 128> pathQuads;
	};

public:
	EnemyHandler() = default;

	void update();
	void updateDraw(double deltaTime);

	void inflictDamage(double damageX, double damageY);
	void populateLevel();

	void setDrawDebug(bool draw);

private:
	double currentTime{};

	FixedVector<Enemy, 512> enemies;
	double lastEnemySpawnTime{};

	bool isDebugRender{};
};

export inline EnemyHandler enemyHandler;
