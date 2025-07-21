export module Menu;

export import Screen;
export import Background;

//Class for pause menu
export class Menu : public Screen
{
public:
	enum ButtonType
	{
		eResume,
		eAbandonGame,
		eSaveAndQuit,
		COUNT
	};

	Menu() = default;
	void init();

	void onButtonPressed(size_t index);

	void setVisible(bool visible);

private:
	std::array<Label, COUNT> buttons;
};