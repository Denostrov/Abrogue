module Abrogue:PlayArea;

import :ScreenComponent;
import :Item;

enum class PlayAreaButtonType
{
    ePause,
    eHealth,
    eNutrition,
    eGold,
    eInventory,
    eInventorySlotFirst = eInventory + 1,
    eInventorySlotLast = eInventorySlotFirst + 19,
    eDepth,
    eSearch,
    COUNT
};

enum class PlayAreaTabButtonType
{
    eDebug,
    eDiscoveries,
    eMenu,
    COUNT
};

class PlayArea : public ScreenComponent<PlayArea, EmptyEnumType, PlayAreaButtonType, PlayAreaTabButtonType, QuadLayer::eMap>
{
public:
    void init();

    void onButtonPressed(ButtonType type);
    void onTabButtonPressed(TabButtonType type) const;

    void updateInventory(FixedVector<Item, 20> const& inventory, std::int64_t gold);

    [[nodiscard]] bool getPaused() const { return buttons[ButtonType::ePause].getPressed(); }
    void setPaused(bool paused);

    void setPlayerHealth(double percentage);

    void refreshLabels();
};