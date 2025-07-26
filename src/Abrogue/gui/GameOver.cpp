module GameOver;

using namespace std::literals;

void GameOver::init()
{
	labels[LabelType::eGameOver].init(""sv, 58, 15, QuadPool::ePopup);
	labels[LabelType::ePressSpace].init("Press [ESC] to quit"sv, 52, 16, QuadPool::ePopup);
}

void GameOver::setWinner(bool winner)
{
	labels[LabelType::eGameOver].setPosition(winner ? 49 : 58, 15);
	labels[LabelType::eGameOver].setText(winner ? "Number One Victory Royale"sv : "joever"sv);
}
