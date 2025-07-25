module GameOver;

using namespace std::literals;

void GameOver::init()
{
	labels[(std::size_t)LabelType::eGameOver].init(""sv, 58, 15, QuadPool::ePopup);
	labels[(std::size_t)LabelType::ePressSpace].init("Press [ESC] to quit"sv, 52, 16, QuadPool::ePopup);
}

void GameOver::setWinner(bool winner)
{
	labels[(std::size_t)LabelType::eGameOver].setPosition(winner ? 49 : 58, 15);
	labels[(std::size_t)LabelType::eGameOver].setText(winner ? "Number One Victory Royale"sv : "joever"sv);
}
