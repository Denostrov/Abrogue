module OptionsMenu;

import Configuration;

using namespace std::literals;

void OptionsMenu::init()
{
	using enum ButtonType;

	labels[(std::size_t)LabelType::eControls].init("Controls"sv, 60, 15, QuadPool::ePopup);

	buttons[(std::size_t)ePauseControl].init("Pause"sv, 62, 17, QuadPool::ePopup);
}

void OptionsMenu::onButtonPressed(ButtonType index)
{
	using enum ButtonType;
}