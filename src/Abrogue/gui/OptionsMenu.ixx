export module OptionsMenu;

export import Screen;

enum class LabelType
{
	eControls,
	COUNT
};

enum class ButtonType
{
	eMoveUp,
	eMoveDown,
	eMoveLeft,
	eMoveRight,
	eAttack,
	ePause,
	eSearch,
	eDiscoveries,
	eMenu,
	eDebug,
	eStopTime,
	eStepTime,
	eResetToDefault,
	eApplyChanges,
	COUNT
};

export class OptionsMenu: public ScreenComponent<OptionsMenu, LabelType, ButtonType>
{
public:
	OptionsMenu() = default;
	void init();

	void onButtonPressed(ButtonType type);

	void refreshLabels();
};