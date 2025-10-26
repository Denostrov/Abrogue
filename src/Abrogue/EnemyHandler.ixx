module Abrogue:EnemyHandler;

import :PhysicsComponent;
import :Player;
import :Weapon;

/*
 * EnemyHandler - class for handling spawning of enemies
 */
class EnemyHandler
{
    class Enemy : public PhysicsComponent
    {
    public:
        enum class State
        {
            eSleeping,
            eWandering,
            eHunting
        };

        Enemy() = default;
        template <bool isDebug>
        Enemy(EnemyData const& data, double positionX, double positionY, State initialState, BoolSequence<isDebug>);

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

        QuadReference<QuadLayer::eEntity> quad;

        QuadReference<QuadLayer::eEntity> stateQuad;
        FixedVector<QuadReference<QuadLayer::eEntity>, 128> pathQuads;
    };

public:
    EnemyHandler() = default;

    void update();
    void updateDraw(double deltaTime);

    void inflictDamage(double damageX, double damageY);
    void populateLevel();

    void setDrawDebug(bool draw);

private:
    template <bool isDebug>
    bool spawnEnemy();

    double currentTime{};

    FixedVector<Enemy, 512> enemies;
    double lastEnemySpawnTime{};

    bool isDrawDebug{};
};
inline EnemyHandler enemyHandler;