module GameOver;

using namespace std::literals;

void GameOver::init()
{
	gameOverLabel.init(""sv, 58, 15, QuadPool::ePopup);
	pressSpaceLabel.init("Press [ESC] to quit"sv, 52, 16, QuadPool::ePopup);
}

void GameOver::setVisible(bool visible)
{
	gameOverLabel.setVisible(visible);
	pressSpaceLabel.setVisible(visible);
}

void GameOver::setWinner(bool winner)
{
	gameOverLabel.setPosition(winner ? 49 : 58, 15);
	gameOverLabel.setText(winner ? "Number One Victory Royale"sv : "joever"sv);
}
