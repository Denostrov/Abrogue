export module Discoveries;

export import Screen;

//Class for discovered items menu
export class Discoveries : public Screen
{
public:
	Discoveries() = default;
	void init();

	void onButtonPressed(std::size_t index) {}

	void setVisible(bool visible);

private:
	Label title;
};