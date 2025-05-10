export module DebugMenu;

export import Screen;
export import Background;

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

	void updateDraw(double deltaTime) final;
	void setVisible(bool visible) final;

	void resetToDefault();
	void toggleStopTime() { onButtonPressed(ButtonType::eStopTime); }
	void toggleStepTime() { onButtonPressed(ButtonType::eStepTime); }

private:
	void onButtonPressed(std::size_t index) final;

	std::array<Label, COUNT> labels;

	Background background;
};