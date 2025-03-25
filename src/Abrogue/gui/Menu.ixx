export module Menu;

export import Label;

export class Menu
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

	auto& getButtons() { return buttons; }

	void setVisible(bool visible);

	void onButtonPressed(ButtonType type);

private:
	std::array<Label, COUNT> buttons;
	QuadPool::Reference background;
};