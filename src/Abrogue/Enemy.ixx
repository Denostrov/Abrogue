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
		template<bool isDrawDebug>
		Enemy(EnemyData const& data, double positionX, double positionY, State initialState, IsDebugTag<isDrawDebug>)
			:PhysicsComponent(positionX, positionY, 0.45, 0.45, 0.45, 0.45), color(data.color), state(initialState)
		{
			auto [x, y] = getPosition();
			quad = quadPool.insert(QuadData{{Constants::mapOffset + x, y},
											{color.getPacked(), color.getTransparentPacked()}, data.symbol},
								   QuadPool::eEntity);

			if constexpr(isDrawDebug)
				updateDrawDebug();

			setMass(data.mass);
			setMaxVelocity(data.speed);

			weapon.init(data.weaponType, data.weaponColor, data.damage, data.attackTime, false);
		}

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
	template<bool isDebug>
	bool spawnEnemy()
	{
		auto enemyDataOpt = configuration.getSuitableEnemy();
		if(!enemyDataOpt)
			return false;

		auto const& spawnRoom = map.getRandomRoom();
		std::int64_t spawnX = spawnRoom.originX + mapRandom.generate() % spawnRoom.width;
		std::int64_t spawnY = spawnRoom.originY + mapRandom.generate() % spawnRoom.height;

		if(map.getTileInLineOfSight(spawnX, spawnY))
			return false;

		enemies.emplace_back(*enemyDataOpt, spawnX + 0.5, spawnY + 0.5, Enemy::State::eSleeping, IsDebug<isDebug>);

		return true;
	}

	double currentTime{};

	FixedVector<Enemy, 512> enemies;
	double lastEnemySpawnTime{};

	bool isDrawDebug{};
};

export inline EnemyHandler enemyHandler;
