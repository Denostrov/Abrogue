export module Discoveries;

export import Screen;
export import Background;

export class Discoveries : public Screen
{
public:
	Discoveries() = default;

	void init();

	void updateDraw(double deltaTime) final;
	void setVisible(bool visible) final;

private:
	Label title;

	Background background;
};