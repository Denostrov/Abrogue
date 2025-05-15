export module Item;

export import QuadPool;
export import PhysicsComponent;

export class Item: public PhysicsComponent
{
public:
	enum class Type
	{
		eGold,
		eFood,
		eAmulet,
		COUNT
	};

	Item() = default;
	Item(Type newType, double x, double y);

	void update();
	void updateDraw(double deltaTime);

private:
	static constexpr std::array<std::uint32_t, (size_t)Type::COUNT> typeGlyphs{42, 59, 157};

	Type type{};

	QuadPool::Reference quad;
};