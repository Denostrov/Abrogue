module GameOver;

import Configuration;

using namespace std::literals;

void GameOver::init()
{
	labels[LabelType::eGameOver].init(""sv, 58, 15, QuadPool::ePopup);
	labels[LabelType::ePressSpace].init(""sv, 52, 16, QuadPool::ePopup);

	refreshLabels();
}

void GameOver::setWinner(bool winner)
{
	labels[LabelType::eGameOver].setPosition(winner ? 49 : 58, 15);
	labels[LabelType::eGameOver].setText(winner ? "Number One Victory Royale"sv : "joever"sv);
}

void GameOver::refreshLabels()
{
	FixedString<32> labelText;

	labels[LabelType::ePressSpace].setText(labelText.fill("Press "sv, configuration.getInputControlName(InputControlType::eMenu), " to quit"sv));
}