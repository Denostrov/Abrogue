export module Discoveries;

export import Screen;

//Enum for discovery labels
enum class LabelType
{
	eTitle,
	COUNT
};

//Class for a popup menu with discovered items
export class Discoveries : public ScreenComponent<Discoveries, LabelType>
{
public:
	Discoveries() = default;
	void init();
};