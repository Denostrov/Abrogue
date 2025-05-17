export module PlayArea;

export import Screen;
export import Item;

export class PlayArea : public Screen
{
public:
	enum class ButtonType
	{
		//Normal buttons
		ePause,
		eHealth,
		eNutrition,
		eGold,
		eInventory,
		eInventorySlotFirst = eInventory + 1,
		eInventorySlotLast = eInventorySlotFirst + 19,
		eDepth,
		eSearch,
		//Tab buttons
		eDebug,
		eDiscoveries,
		eMenu,
		COUNT
	};

	PlayArea() = default;
	void init();

	void updateInventory(FixedVector<Item, 20> const& inventory, std::int64_t gold);

	void setVisible(bool visible) final;

	bool getPaused() const { return buttons[(size_t)ButtonType::ePause].getPressed(); }
	void setPaused(bool paused);

	void setTabButtonPressed(ButtonType type);

	void setPlayerHealth(double percentage);

private:
	void onButtonPressed(size_t index) final;

	std::array<Label, (size_t)ButtonType::COUNT> buttons;
	ButtonType pressedTabButton{ButtonType::COUNT};
};
