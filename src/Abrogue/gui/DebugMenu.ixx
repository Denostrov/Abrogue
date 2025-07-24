export module DebugMenu;

export import Screen;

enum class ButtonType
{
	eTitle,
	eStopTime,
	eStepTime,
	eShowDamage,
	eShowViewcone,
	COUNT
};

export class DebugMenu: public ScreenComponent<DebugMenu, ButtonType>
{
public:
	DebugMenu() = default;
	void init();

	void onButtonPressed(ButtonType type);

	void setVisible(bool visible);

	void resetToDefault();

	void toggleStopTime() { onButtonPressed(ButtonType::eStopTime); }
	void toggleStepTime() { onButtonPressed(ButtonType::eStepTime); }
};