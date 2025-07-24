module PauseMenu;

import GUI;
import Game;

using namespace std::literals;

void PauseMenu::init()
{
	using enum ButtonType;

	buttons[(std::size_t)eResume].init("Resume"sv, 60, 15, QuadPool::ePopup);
	buttons[(std::size_t)eOptions].init("Options"sv, 60, 16, QuadPool::ePopup);
	buttons[(std::size_t)eAbandonGame].init("Abandon game"sv, 57, 17, QuadPool::ePopup);
	buttons[(std::size_t)eSaveAndQuit].init("Save and quit"sv, 57, 18, QuadPool::ePopup);
}

void PauseMenu::setVisible(bool visible)
{
	for(auto& button : buttons)
		button.setVisible(visible);
}

void PauseMenu::onButtonPressed(ButtonType type)
{
	using enum ButtonType;

	if(type == eResume)
		gui.showPlayArea();
	else if(type == eOptions)
		gui.showOptionsMenu();
	else if(type == eAbandonGame || type == eSaveAndQuit)
		game.quitToMainMenu();
}
