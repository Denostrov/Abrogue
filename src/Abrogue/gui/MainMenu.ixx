export module MainMenu;

export import Screen;

export class MainMenu : public Screen
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

	void setVisible(bool visible) final;

private:
	void onButtonPressed(size_t index) final;

	std::array<Label, (size_t)ButtonType::COUNT> buttons;
};