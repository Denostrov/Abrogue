export module PauseMenu;

export import Screen;
export import Background;

enum class ButtonType
{
	eResume,
	eOptions,
	eAbandonGame,
	eSaveAndQuit,
	COUNT
};

//Class for pause menu
export class PauseMenu: public ScreenComponent<PauseMenu, EmptyEnumType, ButtonType>
{
public:
	PauseMenu() = default;
	void init();

	void onButtonPressed(ButtonType type);
};