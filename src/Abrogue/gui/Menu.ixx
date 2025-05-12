export module Menu;

export import Screen;
export import Background;

//Class for pause menu
export class Menu : public Screen
{
public:
	//Enum for pressable buttons
	enum ButtonType
	{
		eResume,
		eAbandonGame,
		eSaveAndQuit,
		COUNT
	};

	Menu() = default;
	void init();

	void setVisible(bool visible) final;

private:
	void onButtonPressed(size_t index) final;

	std::array<Label, COUNT> buttons;
};