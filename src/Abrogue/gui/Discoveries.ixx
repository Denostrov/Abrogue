export module Discoveries;

export import Screen;

//Class for discovered items menu
export class Discoveries : public Screen
{
public:
	Discoveries() = default;
	void init();

	void setVisible(bool visible) final;

private:
	Label title;
};