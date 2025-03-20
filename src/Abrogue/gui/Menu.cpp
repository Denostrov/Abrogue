module Menu;

import GameSystems;

void Menu::init()
{
	buttons[eResume].init("Resume", 60, 15, QuadPool::ePopup);
	buttons[eAbandonGame].init("Abandon game", 57, 16, QuadPool::ePopup);
	buttons[eSaveAndQuit].init("Save and quit", 57, 17, QuadPool::ePopup);
	background = quadPool.insert(QuadData{{0.9f, 0.5f}, 200.0f, {Helpers::packColor(0, 0, 0, 0), Helpers::packColor(0, 0, 0, 0)}, ' '}, QuadPool::ePopupBackground);
}

void Menu::setVisible(bool visible)
{
	for(auto& button : buttons)
		button.setVisible(visible);

	background.setBackgroundColor(visible ? Helpers::packColor(0, 0, 0, 240) : Helpers::packColor(0, 0, 0, 0));
}