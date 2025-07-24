export module OptionsMenu;

export import Screen;

enum class ButtonType
{
	ePauseControl,
	COUNT
};

export class OptionsMenu: public ScreenComponent<OptionsMenu, ButtonType>
{
public:
	OptionsMenu() = default;
	void init();

	void onButtonPressed(ButtonType index);

	void setVisible(bool visible);

private:
	Label controlsLabel;
};