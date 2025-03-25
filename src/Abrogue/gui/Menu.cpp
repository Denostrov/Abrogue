module Menu;

import GameSystems;

void Menu::init()
{
	buttons[eResume].init("Resume", 60, 15, QuadPool::ePopup);
	buttons[eAbandonGame].init("Abandon game", 57, 16, QuadPool::ePopup);
	buttons[eSaveAndQuit].init("Save and quit", 57, 17, QuadPool::ePopup);
}

void Menu::setVisible(bool visible)
{
	for(auto& button : buttons)
		button.setVisible(visible);

	if(visible)
	{
		QuadData backgroundQuad{{0.8889f, 0.5f}, {Helpers::packColor(0, 0, 0, 0), Helpers::packColor(0, 0, 0, 240)}, ' '};
		backgroundQuad.setScale(128.0f, 36.0f);
		background = quadPool.insert(backgroundQuad, QuadPool::ePopupBackground);
	}
	else
		background = QuadPool::Reference();
}

void Menu::onButtonPressed(ButtonType type)
{
	if(type == ButtonType::eResume)
		gui.toggleMenu();
	else if(type == ButtonType::eAbandonGame || type == ButtonType::eSaveAndQuit)
		game.quitToMenu();
}
