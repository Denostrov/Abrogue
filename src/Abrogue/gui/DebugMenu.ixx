export module DebugMenu;

export import Screen;

export class DebugMenu : public Screen
{
public:
	DebugMenu() = default;

	void init();

	void setVisible(bool visible) final;

private:
	Label title;
	QuadPool::Reference background;
};