module DebugMenu;

import GameSystems;

void DebugMenu::init()
{
	labels[eTitle].init("Debug Options", 56, 6, QuadPool::ePopup);
	labels[eStopTime].init("Stop time[Num7]", 40, 8, QuadPool::ePopup);
	labels[eStepTime].init("Step forward[Num8]", 38, 10, QuadPool::ePopup);
	labels[eShowDamage].init("Show damage", 70, 8, QuadPool::ePopup);

	pressableButtons = labels;
}

void DebugMenu::updateDraw(double deltaTime)
{
	background.updateDraw(deltaTime);
}

void DebugMenu::setVisible(bool visible)
{
	for(auto& label : labels)
		label.setVisible(visible);
	
	background.setVisible(visible);
}

void DebugMenu::resetToDefault()
{
	if(labels[eStopTime].getPressed())
		onButtonPressed((size_t)eStopTime);

	if(labels[eShowDamage].getPressed())
		onButtonPressed((size_t)eShowDamage);
}

void DebugMenu::onButtonPressed(std::size_t index)
{
	auto type = (ButtonType)index;

	if(type == eStopTime)
	{
		game.setSpeedMultiplier(labels[eStopTime].getPressed() ? 1.0 : 0.0);
		labels[eStopTime].togglePressed();
	}
	else if(type == eStepTime)
	{
		if(labels[eStopTime].getPressed())
			game.advanceStep();
	}
	else if(type == eShowDamage)
	{
		Weapon::setDrawDebug(!labels[eShowDamage].getPressed());
		labels[eShowDamage].togglePressed();
	}
}
