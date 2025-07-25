export module MainMenu;

export import Screen;

enum class ButtonType
{
	eStartGame,
	eOptions,
	eQuitToDesktop,
	COUNT
};

//Class for main menu screen
export class MainMenu: public ScreenComponent<MainMenu, EmptyEnumType, ButtonType>
{
public:
	MainMenu() = default;
	void init();

	void onButtonPressed(ButtonType type);
};