export module Discoveries;

export import Screen;

export class Discoveries : public Screen
{
public:
	Discoveries() = default;

	void init();

	void setVisible(bool visible) final;

private:
	Label title;
	QuadPool::Reference background;
};