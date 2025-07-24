module MainMenu;

import Game;
import GUI;

using namespace std::literals;

void MainMenu::init()
{
	using enum ButtonType;

	buttons[(std::size_t)eStartGame].init("New game"sv, 120, 30, QuadPool::eMap);
	buttons[(std::size_t)eOptions].init("Options"sv, 121, 31, QuadPool::eMap);
	buttons[(std::size_t)eQuitToDesktop].init("Quit to desktop"sv, 113, 32, QuadPool::eMap);
}

void MainMenu::setVisible(bool visible)
{
	for(auto& button : buttons)
		button.setVisible(true);
}

void MainMenu::onButtonPressed(ButtonType type)
{
	using enum ButtonType;

	if(type == eStartGame)
		game.startGame();
	else if(type == eOptions)
		gui.showOptionsMenu();
	else if(type == eQuitToDesktop)
		game.quitToDesktop();
}
