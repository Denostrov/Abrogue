export module PlayArea;

export import Screen;
export import Item;

enum class ButtonType
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

enum class TabButtonType
{
	eDebug,
	eDiscoveries,
	eMenu,
	COUNT
};

export class PlayArea : public ScreenComponent<PlayArea, ButtonType, TabButtonType>
{
public:
	PlayArea() = default;
	void init();

	void onButtonPressed(ButtonType type);
	void onTabButtonPressed(TabButtonType type);

	void updateInventory(FixedVector<Item, 20> const& inventory, std::int64_t gold);

	void setVisible(bool visible);

	bool getPaused() const { return buttons[(size_t)ButtonType::ePause].getPressed(); }
	void setPaused(bool paused);

	void setPlayerHealth(double percentage);
};
