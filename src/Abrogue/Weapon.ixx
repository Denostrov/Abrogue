module Abrogue:Weapon;

import :QuadPool;

/*
 * Weapon - class for handling attacks and dealing damage
 */
class Weapon
{
public:
    Weapon() = default;
    void init(WeaponType newType, Color newColor, std::int64_t newDamage, double newAttackTime, bool friendly);

    bool getIsAttacking() const { return attackTimer > 0.0; }

    void update(double positionX, double positionY);
    void updateDraw(double positionX, double positionY);

    void startAttack(double positionX, double positionY, double targetPositionX, double targetPositionY);

    static void setDrawDebug(bool draw) { drawDebugInfo = draw; }

private:
    inline static bool drawDebugInfo{};

    WeaponType type;
    Color color;
    std::int64_t damage{};
    double attackTime{};
    double drawOffset{};
    bool isFriendly{};

    QuadReference<QuadLayer::eItem> weaponReference;
    double attackTimer{};
    double attackAngleCos{};
    double attackAngleSin{};

    QuadReference<QuadLayer::eMapOverlay> damageReference;
};