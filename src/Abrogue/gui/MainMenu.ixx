export module MainMenu;

export import Screen;

//Class for main menu screen
export class MainMenu: public Screen
{
public:
	enum ButtonType
	{
		eStartGame,
		eQuitToDesktop,
		COUNT
	};

	MainMenu() = default;
	void init();

	void onButtonPressed(size_t index);

	void setVisible(bool visible);

private:
	std::array<Label, (size_t)ButtonType::COUNT> buttons;
};