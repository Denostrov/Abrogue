module OptionsMenu;

import Configuration;

using namespace std::literals;

void OptionsMenu::init()
{
	using enum ButtonType;

	controlsLabel.init("Controls"sv, 60, 15, QuadPool::ePopup);
	buttons[(std::size_t)ePauseControl].init("Pause"sv, 62, 17, QuadPool::ePopup);

	pressableButtons = buttons;
}

void OptionsMenu::onButtonPressed(std::size_t index)
{
	using enum ButtonType;
}

void OptionsMenu::setVisible(bool visible)
{
	controlsLabel.setVisible(visible);
	for(auto& button : buttons)
		button.setVisible(visible);
}