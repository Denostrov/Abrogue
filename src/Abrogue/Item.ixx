module Abrogue:Item;

import :PhysicsComponent;
import :QuadPool;

/*
 * Item - class for handling items
 */
class Item : public PhysicsComponent
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

    [[nodiscard]] Type getType() const { return type; }
    [[nodiscard]] FixedString<32> getName() const;

    void setVisible(bool visible);

private:
    static constexpr std::array<std::uint32_t, (std::size_t)Type::COUNT> typeGlyphs{42, 59, 157};

    Type type{};

    QuadReference<QuadLayer::eItem> quad;
};