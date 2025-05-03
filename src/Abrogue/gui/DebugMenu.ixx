export module DebugMenu;

export import Screen;

//Class for debug options menu
export class DebugMenu : public Screen
{
	//Enum for labels
	enum ButtonType
	{
		eTitle,
		eStopTime,
		eStepTime,
		eShowDamage,
		COUNT
	};

public:
	DebugMenu() = default;
	//Initialize labels
	void init();

	void setVisible(bool visible) final;

	void toggleStopTime() { onButtonPressed(ButtonType::eStopTime); }
	void toggleStepTime() { onButtonPressed(ButtonType::eStepTime); }

private:
	void onButtonPressed(std::size_t index) final;

	std::array<Label, COUNT> labels;

	QuadPool::Reference background;
};