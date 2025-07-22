export module PauseMenu;

export import Screen;
export import Background;

//Class for pause menu
export class PauseMenu: public Screen
{
public:
	enum ButtonType
	{
		eResume,
		eOptions,
		eAbandonGame,
		eSaveAndQuit,
		COUNT
	};

	PauseMenu() = default;
	void init();

	void onButtonPressed(size_t index);

	void setVisible(bool visible);

private:
	std::array<Label, COUNT> buttons;
};