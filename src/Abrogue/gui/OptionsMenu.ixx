export module OptionsMenu;

export import Screen;

export class OptionsMenu: public Screen
{
public:
	enum class ButtonType
	{
		ePauseControl,
		COUNT
	};

	OptionsMenu() = default;
	void init();

	void onButtonPressed(std::size_t index);

	void setVisible(bool visible);

private:
	Label controlsLabel;
	std::array<Label, (std::size_t)ButtonType::COUNT> buttons;
};