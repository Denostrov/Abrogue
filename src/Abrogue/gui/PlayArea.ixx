export module PlayArea;

export import Screen;

export class PlayArea : public Screen
{
public:
	enum class ButtonType
	{
		//Normal buttons
		ePause,
		eHealth,
		eNutrition,
		eSearch,
		eInventory,
		//Tab buttons
		eDebug,
		eDiscoveries,
		eMenu,
		COUNT
	};

	PlayArea() = default;

	void init();

	void setVisible(bool visible) final;

	bool getPaused() const { return buttons[(size_t)ButtonType::ePause].getPressed(); }
	void setPaused(bool paused);

private:
	void onButtonPressed(size_t index) final;

	std::array<Label, (size_t)ButtonType::COUNT> buttons;
	ButtonType pressedTabButton{ButtonType::COUNT};
};
