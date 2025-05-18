module DebugMenu;

import Game;
import Weapon;
import Map;

using namespace std::literals;

void DebugMenu::init()
{
	labels[eTitle].init("Debug Options"sv, 56, 6, QuadPool::ePopup);
	labels[eStopTime].init("Stop time[Num7]"sv, 40, 8, QuadPool::ePopup);
	labels[eStepTime].init("Step forward[Num8]"sv, 38, 10, QuadPool::ePopup);
	labels[eShowDamage].init("Show damage"sv, 70, 8, QuadPool::ePopup);
	labels[eShowViewcone].init("Show viewcone"sv, 70, 10, QuadPool::ePopup);

	pressableButtons = labels;
}

void DebugMenu::setVisible(bool visible)
{
	for(auto& label : labels)
		label.setVisible(visible);
}

void DebugMenu::resetToDefault()
{
	//Reset buttons to unpressed state
	if(labels[eStopTime].getPressed())
		onButtonPressed((size_t)eStopTime);

	if(labels[eShowDamage].getPressed())
		onButtonPressed((size_t)eShowDamage);

	if(labels[eShowViewcone].getPressed())
		onButtonPressed((size_t)eShowViewcone);
}

void DebugMenu::onButtonPressed(std::size_t index)
{
	auto type = (ButtonType)index;

	if(type == eStopTime)
	{
		labels[eStopTime].togglePressed();
		game.setSpeedMultiplier(labels[eStopTime].getPressed() ? 0.0 : 1.0);
	}
	else if(type == eStepTime && labels[eStopTime].getPressed())
	{
		game.advanceStep();
	}
	else if(type == eShowDamage)
	{
		labels[eShowDamage].togglePressed();
		Weapon::setDrawDebug(labels[eShowDamage].getPressed());
	}
	else if(type == eShowViewcone)
	{
		labels[eShowViewcone].togglePressed();
		Map::setDrawDebugViewcone(labels[eShowViewcone].getPressed());
	}
}
