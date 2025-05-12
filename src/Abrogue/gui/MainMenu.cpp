module MainMenu;

import GameSystems;

using namespace std::literals;

void MainMenu::init()
{
	buttons[eStartGame].init("New game"sv, 120, 30, QuadPool::eMap);
	buttons[eQuitToDesktop].init("Quit to desktop"sv, 113, 31, QuadPool::eMap);

	pressableButtons = buttons;
}

void MainMenu::setVisible(bool visible)
{
	for(auto& button : buttons)
		button.setVisible(true);
}

void MainMenu::onButtonPressed(size_t index)
{
	auto type = (ButtonType)index;
	if(type == eStartGame)
		game.startGame();
	else if(type == eQuitToDesktop)
		game.quitToDesktop();
}
