export module Discoveries;

export import Screen;

enum class LabelType
{
	eTitle,
	COUNT
};

//Class for discovered items menu
export class Discoveries : public ScreenComponent<Discoveries, LabelType>
{
public:
	Discoveries() = default;
	void init();
};