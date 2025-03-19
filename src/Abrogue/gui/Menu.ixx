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

	void setVisible(bool visible);

private:
	std::array<Label, COUNT> buttons;
	QuadPool::Reference background;
};