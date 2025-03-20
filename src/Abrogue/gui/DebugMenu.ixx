export module DebugMenu;

export import Label;

export class DebugMenu
{
public:
	DebugMenu() = default;

	void init();

	void setVisible(bool visible);

private:
	Label title;
	QuadPool::Reference background;
};