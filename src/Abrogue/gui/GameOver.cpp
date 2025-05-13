module GameOver;

using namespace std::literals;

void GameOver::init()
{
	gameOverLabel.init("you ded..."sv, 58, 15, QuadPool::ePopup);
	pressSpaceLabel.init("Press [ESC] to quit"sv, 52, 16, QuadPool::ePopup);
}

void GameOver::setVisible(bool visible)
{
	gameOverLabel.setVisible(visible);
	pressSpaceLabel.setVisible(visible);
}
