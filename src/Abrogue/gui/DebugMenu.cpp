module DebugMenu;

import Game;
import Weapon;
import Map;

using namespace std::literals;

void DebugMenu::init()
{
	using enum ButtonType;

	buttons[(std::size_t)eTitle].init("Debug Options"sv, 56, 6, QuadPool::ePopup);
	buttons[(std::size_t)eStopTime].init("Stop time[Num7]"sv, 40, 8, QuadPool::ePopup);
	buttons[(std::size_t)eStepTime].init("Step forward[Num8]"sv, 38, 10, QuadPool::ePopup);
	buttons[(std::size_t)eShowDamage].init("Show damage"sv, 70, 8, QuadPool::ePopup);
	buttons[(std::size_t)eShowViewcone].init("Show viewcone"sv, 70, 10, QuadPool::ePopup);
}

void DebugMenu::setVisible(bool visible)
{
	for(auto& button : buttons)
		button.setVisible(visible);
}

void DebugMenu::resetToDefault()
{
	using enum ButtonType;

	if(buttons[(std::size_t)eStopTime].getPressed())
		onButtonPressed(eStopTime);

	if(buttons[(std::size_t)eShowDamage].getPressed())
		onButtonPressed(eShowDamage);

	if(buttons[(std::size_t)eShowViewcone].getPressed())
		onButtonPressed(eShowViewcone);
}

void DebugMenu::onButtonPressed(ButtonType type)
{
	using enum ButtonType;

	if(type == eStopTime)
	{
		buttons[(std::size_t)eStopTime].togglePressed();
		game.setSpeedMultiplier(buttons[(std::size_t)eStopTime].getPressed() ? 0.0 : 1.0);
	}
	else if(type == eStepTime && buttons[(std::size_t)eStopTime].getPressed())
	{
		game.advanceStep();
	}
	else if(type == eShowDamage)
	{
		buttons[(std::size_t)eShowDamage].togglePressed();
		Weapon::setDrawDebug(buttons[(std::size_t)eShowDamage].getPressed());
	}
	else if(type == eShowViewcone)
	{
		buttons[(std::size_t)eShowViewcone].togglePressed();
		Map::setDrawDebugViewcone(buttons[(std::size_t)eShowViewcone].getPressed());
	}
}
