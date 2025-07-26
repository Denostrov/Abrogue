module OptionsMenu;

import InputHandler;

using namespace std::literals;

void OptionsMenu::init()
{
	using enum ButtonType;

	labels[LabelType::eControls].init("Controls"sv, 60, 5, QuadPool::ePopup);

	buttons[eMoveUp].init(""sv, 58, 7, QuadPool::ePopup);
	buttons[eMoveDown].init(""sv, 58, 9, QuadPool::ePopup);
	buttons[eMoveLeft].init(""sv, 58, 11, QuadPool::ePopup);
	buttons[eMoveRight].init(""sv, 58, 13, QuadPool::ePopup);
	buttons[eAttack].init(""sv, 58, 15, QuadPool::ePopup);
	buttons[ePause].init(""sv, 58, 17, QuadPool::ePopup);
	buttons[eSearch].init(""sv, 58, 19, QuadPool::ePopup);
	buttons[eDiscoveries].init(""sv, 58, 21, QuadPool::ePopup);
	buttons[eMenu].init(""sv, 58, 23, QuadPool::ePopup);
	buttons[eDebug].init(""sv, 58, 25, QuadPool::ePopup);
	buttons[eStopTime].init(""sv, 58, 27, QuadPool::ePopup);
	buttons[eStepTime].init(""sv, 58, 29, QuadPool::ePopup);

	buttons[eResetToDefault].init("Reset To Default"sv, 58, 32, QuadPool::ePopup);
	buttons[eApplyChanges].init("Apply changes"sv, 80, 34, QuadPool::ePopup);

	refreshLabels();
}

void OptionsMenu::onButtonPressed(ButtonType type)
{
	using enum ButtonType;

	if(type <= eStepTime)
		buttons[type].setPressed(true);

	if(type == eMoveUp)
		inputHandler.setChangingControlType(InputControlType::eMoveUp);
	else if(type == eMoveDown)
		inputHandler.setChangingControlType(InputControlType::eMoveDown);
	else if(type == eMoveLeft)
		inputHandler.setChangingControlType(InputControlType::eMoveLeft);
	else if(type == eMoveRight)
		inputHandler.setChangingControlType(InputControlType::eMoveRight);
	else if(type == eAttack)
		inputHandler.setChangingControlType(InputControlType::eAttack);
	else if(type == ePause)
		inputHandler.setChangingControlType(InputControlType::ePause);
	else if(type == eSearch)
		inputHandler.setChangingControlType(InputControlType::eSearch);
	else if(type == eDiscoveries)
		inputHandler.setChangingControlType(InputControlType::eDiscoveries);
	else if(type == eMenu)
		inputHandler.setChangingControlType(InputControlType::eMenu);
	else if(type == eDebug)
		inputHandler.setChangingControlType(InputControlType::eDebug);
	else if(type == eStopTime)
		inputHandler.setChangingControlType(InputControlType::eStopTime);
	else if(type == eStepTime)
		inputHandler.setChangingControlType(InputControlType::eStepTime);
	else if(type == eApplyChanges)
		configuration.saveOptions();
}

void OptionsMenu::refreshLabels()
{
	using enum ButtonType;

	FixedString<32> labelText;

	buttons[eMoveUp].setText(labelText.fill("Move Up"sv, configuration.getInputControlName(InputControlType::eMoveUp)));
	buttons[eMoveDown].setText(labelText.fill("Move Down"sv, configuration.getInputControlName(InputControlType::eMoveDown)));
	buttons[eMoveLeft].setText(labelText.fill("Move Left"sv, configuration.getInputControlName(InputControlType::eMoveLeft)));
	buttons[eMoveRight].setText(labelText.fill("Move Right"sv, configuration.getInputControlName(InputControlType::eMoveRight)));
	buttons[eAttack].setText(labelText.fill("Attack"sv, configuration.getInputControlName(InputControlType::eAttack)));
	buttons[ePause].setText(labelText.fill("Pause"sv, configuration.getInputControlName(InputControlType::ePause)));
	buttons[eSearch].setText(labelText.fill("Search"sv, configuration.getInputControlName(InputControlType::eSearch)));
	buttons[eDiscoveries].setText(labelText.fill("Discoveries"sv, configuration.getInputControlName(InputControlType::eDiscoveries)));
	buttons[eMenu].setText(labelText.fill("Menu"sv, configuration.getInputControlName(InputControlType::eMenu)));
	buttons[eDebug].setText(labelText.fill("Debug"sv, configuration.getInputControlName(InputControlType::eDebug)));
	buttons[eStopTime].setText(labelText.fill("Stop Time"sv, configuration.getInputControlName(InputControlType::eStopTime)));
	buttons[eStepTime].setText(labelText.fill("Step Time"sv, configuration.getInputControlName(InputControlType::eStepTime)));

	for(std::size_t i = (std::size_t)eMoveUp; i <= (std::size_t)eStepTime; i++)
		buttons[i].setPressed(false);
}
