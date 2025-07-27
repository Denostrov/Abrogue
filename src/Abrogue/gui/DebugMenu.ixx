export module DebugMenu;

export import Screen;

//Enum for debug menu buttons
enum class ButtonType
{
	eTitle,
	eStopTime,
	eStepTime,
	eShowDamage,
	eShowViewcone,
	COUNT
};

//Class for menu screen with debugging options
export class DebugMenu: public ScreenComponent<DebugMenu, EmptyEnumType, ButtonType>
{
public:
	DebugMenu() = default;
	void init();

	void onButtonPressed(ButtonType type);

	void resetToDefault();

	void refreshLabels();
};