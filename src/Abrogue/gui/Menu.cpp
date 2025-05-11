module Menu;

import GameSystems;

void Menu::init()
{
	buttons[eResume].init("Resume", 60, 15, QuadPool::ePopup);
	buttons[eAbandonGame].init("Abandon game", 57, 16, QuadPool::ePopup);
	buttons[eSaveAndQuit].init("Save and quit", 57, 17, QuadPool::ePopup);

	pressableButtons = buttons;
}

void Menu::setVisible(bool visible)
{
	for(auto& button : buttons)
		button.setVisible(visible);
}

void Menu::onButtonPressed(size_t index)
{
	auto type = (ButtonType)index;

	if(type == ButtonType::eResume)
		gui.toggleMenu();
	else if(type == ButtonType::eAbandonGame || type == ButtonType::eSaveAndQuit)
		gui.quitToMainMenu();
}
