export module MainMenu;

export import Screen;

//Class for main menu screen
export class MainMenu: public Screen
{
public:
	//Enum for pressable buttons
	enum ButtonType
	{
		eStartGame,
		eQuitToDesktop,
		COUNT
	};

	MainMenu() = default;
	//Initialize the screen layout
	void init();

	//Set all buttons visible
	void setVisible(bool visible) final;

private:
	//Handle pressed button
	void onButtonPressed(size_t index) final;

	std::array<Label, (size_t)ButtonType::COUNT> buttons;	//Pressable buttons
};