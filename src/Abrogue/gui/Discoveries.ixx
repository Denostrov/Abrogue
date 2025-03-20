export module Discoveries;

export import Label;

export class Discoveries
{
public:
	Discoveries() = default;

	void init();

	void setVisible(bool visible);

private:
	Label title;
	QuadPool::Reference background;
};