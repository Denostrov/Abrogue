export module Player;

export import PhysicsComponent;
export import QuadPool;
export import Weapon;
export import Item;

export class Player : public PhysicsComponent
{
public:
	Player() = default;
	Player(double velocity);

	void onMousePressed(std::uint32_t x, std::uint32_t y);

	void update();
	void updateDraw(double deltaTime);

	void takeDamage(std::int64_t damage);

	std::int64_t getStealthRange() const { return stealthRange; }

	void setMovement(std::int32_t movementX, std::int32_t movementY);
	void setHealth(std::int64_t newHealth);

	Weapon weapon;
	std::int64_t health{};
	std::int64_t gold{};
	std::int64_t stealthRange{4};

	std::int64_t lastTileX{}, lastTileY{};

	FixedVector<Item, 20> inventory;
	bool hasAmulet{};

	QuadPool::Reference quadReference;
};

export inline Player player;