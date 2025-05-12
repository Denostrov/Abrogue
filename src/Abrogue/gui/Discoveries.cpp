module Discoveries;

import GameSystems;

using namespace std::literals;

void Discoveries::init()
{
	title.init("Discoveries"sv, 56, 6, QuadPool::ePopup);
}

void Discoveries::setVisible(bool visible)
{
	title.setVisible(visible);
}