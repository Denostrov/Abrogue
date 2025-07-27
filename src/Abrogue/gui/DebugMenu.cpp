module DebugMenu;

import Game;
import Weapon;
import Map;

using namespace std::literals;

void DebugMenu::init()
{
	using enum ButtonType;

	buttons[eTitle].init("Debug Options"sv, 56, 6, QuadPool::ePopup);
	buttons[eStopTime].init(""sv, 40, 8, QuadPool::ePopup);
	buttons[eStepTime].init(""sv, 40, 10, QuadPool::ePopup);
	buttons[eShowDamage].init("Show damage"sv, 70, 8, QuadPool::ePopup);
	buttons[eShowViewcone].init("Show viewcone"sv, 70, 10, QuadPool::ePopup);

	refreshLabels();
}

void DebugMenu::resetToDefault()
{
	using enum ButtonType;

	if(buttons[eStopTime].getPressed())
		onButtonPressed(eStopTime);

	if(buttons[eShowDamage].getPressed())
		onButtonPressed(eShowDamage);

	if(buttons[eShowViewcone].getPressed())
		onButtonPressed(eShowViewcone);
}

void DebugMenu::refreshLabels()
{
	using enum ButtonType;

	FixedString<32> labelText;

	buttons[eStopTime].setText(labelText.fill("Stop time"sv, configuration.getInputControlName(InputControlType::eStopTime)));
	buttons[eStepTime].setText(labelText.fill("Step time"sv, configuration.getInputControlName(InputControlType::eStepTime)));
}

void DebugMenu::onButtonPressed(ButtonType type)
{
	using enum ButtonType;

	if(type == eStopTime)
	{
		buttons[eStopTime].togglePressed();
		game.setSpeedMultiplier(buttons[eStopTime].getPressed() ? 0.0 : 1.0);
	}
	else if(type == eStepTime && buttons[eStopTime].getPressed())
	{
		game.advanceStep();
	}
	else if(type == eShowDamage)
	{
		buttons[eShowDamage].togglePressed();
		Weapon::setDrawDebug(buttons[eShowDamage].getPressed());
	}
	else if(type == eShowViewcone)
	{
		buttons[eShowViewcone].togglePressed();
		Map::setDrawDebugViewcone(buttons[eShowViewcone].getPressed());
	}
}
