module Menu;

import GameSystems;

void Menu::init()
{
	buttons[eResume].init("Resume", 60, 16);
	buttons[eAbandonGame].init("Abandon game", 56, 17);
	buttons[eSaveAndQuit].init("Save and quit", 56, 16);
	background = quadPool.insert(QuadData{{0.5f, 0.5f}, 100.0f, {Helpers::packColor(0, 0, 0, 0), Helpers::packColor(0, 0, 0, 128)}, ' '});
}

void Menu::setVisible(bool visible)
{
	for(auto& button : buttons)
		button.setVisible(visible);

	background.setBackgroundColor(visible ? Helpers::packColor(0, 0, 0, 128) : Helpers::packColor(0, 0, 0, 0));
}