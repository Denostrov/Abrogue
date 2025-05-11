module GameOver;

import GameSystems;

void GameOver::init()
{
	gameOverLabel.init("you ded...", 58, 15, QuadPool::ePopup);
	pressSpaceLabel.init("Press [ESC] to quit", 52, 16, QuadPool::ePopup);
}

void GameOver::setVisible(bool visible)
{
	gameOverLabel.setVisible(visible);
	pressSpaceLabel.setVisible(visible);
}
