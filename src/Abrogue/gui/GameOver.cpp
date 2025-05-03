module GameOver;

import GameSystems;

void GameOver::init()
{
	gameOverLabel.init("you ded...", 58, 15, QuadPool::ePopup);
	pressSpaceLabel.init("Press [SPACE] to quit", 52, 16, QuadPool::ePopup);
}

void GameOver::setVisible(bool visible)
{
	gameOverLabel.setVisible(visible);
	pressSpaceLabel.setVisible(visible);

	if(visible)
	{
		QuadData quadData{{16.0f / 9.0f / 2.0f, 0.5f}, {Helpers::packColor(0, 0, 0, 0), Helpers::packColor(0, 0, 0, 240)}, ' '};
		quadData.setScale(128.0f, 36.0f);
		backgroundQuad = quadPool.insert(quadData, QuadPool::ePopupBackground);
	}
	else
		backgroundQuad = QuadPool::Reference();
}
