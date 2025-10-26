module Abrogue:Player;

import :Item;
import :Weapon;

/*
 * Player - class for handling the main character
 */
class Player : public PhysicsComponent
{
public:
    Player() = default;
    Player(double velocity);

    void onMousePressed(std::uint32_t x, std::uint32_t y);

    void update();
    void updateDraw(double deltaTime);

    void takeDamage(std::int64_t damage);

    [[nodiscard]] auto getStealthRange() const { return stealthRange; }

    void setMovement(std::int64_t movementX, std::int64_t movementY);
    void setHealth(std::int64_t newHealth);

    Weapon weapon;
    std::int64_t health{};
    std::int64_t gold{};
    std::int64_t stealthRange{7};

    std::int64_t lastTileX{}, lastTileY{};

    FixedVector<Item, 20> inventory;
    bool hasAmulet{};

    QuadReference<QuadLayer::eEntity> quadReference;
};
inline Player player;