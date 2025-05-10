module Discoveries;

import GameSystems;

void Discoveries::init()
{
	title.init("Discoveries", 56, 6, QuadPool::ePopup);
}

void Discoveries::updateDraw(double deltaTime)
{
	background.updateDraw(deltaTime);
}

void Discoveries::setVisible(bool visible)
{
	title.setVisible(visible);
	background.setVisible(visible);
}