export module DebugMenu;

export import Screen;

//Class for debug options menu
export class DebugMenu : public Screen
{
	enum ButtonType
	{
		eTitle,
		eStopTime,
		eStepTime,
		eShowDamage,
		eShowViewcone,
		COUNT
	};

public:
	DebugMenu() = default;
	void init();

	void onButtonPressed(std::size_t index);

	void setVisible(bool visible);

	void resetToDefault();

	void toggleStopTime() { onButtonPressed(ButtonType::eStopTime); }
	void toggleStepTime() { onButtonPressed(ButtonType::eStepTime); }

private:

	std::array<Label, COUNT> labels;
};