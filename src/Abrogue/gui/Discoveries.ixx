export module Discoveries;

export import Screen;

//Class for discovered items menu
export class Discoveries : public ScreenComponent<Discoveries>
{
public:
	Discoveries() = default;
	void init();

	void setVisible(bool visible);

private:
	Label title;
};