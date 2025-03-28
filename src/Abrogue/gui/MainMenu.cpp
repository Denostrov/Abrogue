module MainMenu;

import GameSystems;

void MainMenu::init()
{
	buttons[eStartGame].init("New game", 120, 30, QuadPool::eMap);
	buttons[eQuitToDesktop].init("Quit to desktop", 113, 31, QuadPool::eMap);

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
