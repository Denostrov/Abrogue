export module OptionsMenu;

export import Screen;

enum class LabelType
{
	eControls,
	COUNT
};

enum class ButtonType
{
	ePauseControl,
	COUNT
};

export class OptionsMenu: public ScreenComponent<OptionsMenu, LabelType, ButtonType>
{
public:
	OptionsMenu() = default;
	void init();

	void onButtonPressed(ButtonType index);
};