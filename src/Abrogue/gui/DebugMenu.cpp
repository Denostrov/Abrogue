module DebugMenu;

import GameSystems;

void DebugMenu::init()
{
	title.init("Debug Options", 56, 8, QuadPool::ePopup);
	background = quadPool.insert(QuadData{{0.9f, 0.5f}, 200.0f, {Helpers::packColor(0, 0, 0, 0), Helpers::packColor(0, 0, 0, 0)}, ' '}, QuadPool::ePopupBackground);
}

void DebugMenu::setVisible(bool visible)
{
	title.setVisible(visible);
	background.setBackgroundColor(visible ? Helpers::packColor(0, 0, 0, 240) : Helpers::packColor(0, 0, 0, 0));
}