export module Menu;

export import Screen;
export import Background;

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

	auto& getButtons() { return buttons; }

	void updateDraw(double deltaTime) final;
	void setVisible(bool visible) final;

private:
	void onButtonPressed(size_t index) final;

	std::array<Label, COUNT> buttons;
	
	Background background;
};