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

void DebugMenu::setVisible(bool visible)
{
	for(auto& label : labels)
		label.setVisible(visible);
	
	if(visible)
	{
		QuadData quadData{{16.0f / 9.0f / 2.0f, 0.5f}, {Helpers::packColor(0, 0, 0, 0), Helpers::packColor(0, 0, 0, 240)}, ' '};
		quadData.setScale(128.0f, 36.0f);
		backgroundQuad = quadPool.insert(quadData, QuadPool::ePopupBackground);
	}
	else
		backgroundQuad = QuadPool::Reference();
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
