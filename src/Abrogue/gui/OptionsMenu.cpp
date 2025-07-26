module OptionsMenu;

import InputHandler;

using namespace std::literals;

void OptionsMenu::init()
{
	using enum ButtonType;

	labels[LabelType::eControls].init("Controls"sv, 60, 11, QuadPool::ePopup);

	buttons[eMoveUp].init("Move Up"sv, 60, 13, QuadPool::ePopup);
	buttons[eMoveDown].init("Move Down"sv, 60, 15, QuadPool::ePopup);
	buttons[eMoveLeft].init("Move Left"sv, 60, 17, QuadPool::ePopup);
	buttons[eMoveRight].init("Move Right"sv, 60, 19, QuadPool::ePopup);
	buttons[eAttack].init("Attack"sv, 60, 21, QuadPool::ePopup);
	buttons[ePauseControl].init("Pause"sv, 60, 23, QuadPool::ePopup);
	buttons[eSearch].init("Search"sv, 60, 25, QuadPool::ePopup);
	buttons[eDiscoveries].init("Discoveries"sv, 60, 27, QuadPool::ePopup);
	buttons[eMenu].init("Menu"sv, 60, 29, QuadPool::ePopup);
	buttons[eDebug].init("Debug"sv, 60, 31, QuadPool::ePopup);
	buttons[eResetToDefault].init("Reset To Default"sv, 56, 33, QuadPool::ePopup);
}

void OptionsMenu::onButtonPressed(ButtonType type)
{
	using enum ButtonType;

	buttons[type].setPressed(true);

	if(type == eMoveUp)
		inputHandler.setChangingControlType(InputControlType::eMoveUp);
}