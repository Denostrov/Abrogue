module PauseMenu;

import GUI;
import Game;

using namespace std::literals;

void PauseMenu::init()
{
	buttons[eResume].init("Resume"sv, 60, 15, QuadPool::ePopup);
	buttons[eOptions].init("Options"sv, 60, 16, QuadPool::ePopup);
	buttons[eAbandonGame].init("Abandon game"sv, 57, 17, QuadPool::ePopup);
	buttons[eSaveAndQuit].init("Save and quit"sv, 57, 18, QuadPool::ePopup);

	pressableButtons = buttons;
}

void PauseMenu::setVisible(bool visible)
{
	for(auto& button : buttons)
		button.setVisible(visible);
}

void PauseMenu::onButtonPressed(size_t index)
{
	auto type = (ButtonType)index;

	if(type == ButtonType::eResume)
		gui.showPlayArea();
	else if(type == ButtonType::eOptions)
		gui.showOptionsMenu();
	else if(type == ButtonType::eAbandonGame || type == ButtonType::eSaveAndQuit)
		game.quitToMainMenu();
}
