module PlayArea;

import GameSystems;

void PlayArea::init()
{
	buttons[(size_t)ButtonType::eHealth].init("Health", 0, 1, QuadPool::eMap);
	buttons[(size_t)ButtonType::eHealth].setBackgroundColor(Constants::healthBackgroundColor, Constants::healthHoverColor);
	buttons[(size_t)ButtonType::eNutrition].init("Nutrition", 7, 1, QuadPool::eMap);
	buttons[(size_t)ButtonType::eNutrition].setBackgroundColor(Constants::nutritionBackgroundColor, Constants::nutritionHoverColor);
	buttons[(size_t)ButtonType::eSearch].init("Search[S]", 11, 35, QuadPool::eMap);
	buttons[(size_t)ButtonType::eInventory].init("Inventory", 0, 5, QuadPool::eMap);
	buttons[(size_t)ButtonType::ePause].init("Pause[SPACE]", 26, 1, QuadPool::eMap);

	buttons[(size_t)ButtonType::eDebug].init("Debug[F3]", 0, 35, QuadPool::eMap);
	buttons[(size_t)ButtonType::eDiscoveries].init("Discoveries[D]", 22, 35, QuadPool::eMap);
	buttons[(size_t)ButtonType::eMenu].init("Menu[ESC]", 38, 35, QuadPool::eMap);

	pressableButtons = buttons;
}

void PlayArea::setVisible(bool visible)
{
	for(auto& button : buttons)
		button.setVisible(visible);
}

void PlayArea::setPaused(bool paused)
{
	if(buttons[(size_t)ButtonType::ePause].getPressed() == paused)
		return;

	buttons[(size_t)ButtonType::ePause].setPressed(paused);
	buttons[(size_t)ButtonType::ePause].setText(paused ? "PAUSED[SPACE]" : "Pause[SPACE]");
	game.setPaused(paused);
}

void PlayArea::setTabButtonPressed(ButtonType type)
{
	if(type < ButtonType::eDebug || pressedTabButton == type)
		return;

	if(pressedTabButton != ButtonType::COUNT)
		buttons[(size_t)pressedTabButton].setPressed(false);

	pressedTabButton = type;

	if(type != ButtonType::COUNT)
		buttons[(size_t)type].setPressed(true);
}

void PlayArea::onButtonPressed(size_t index)
{
	auto type = (ButtonType)index;

	if(type < ButtonType::eDebug)
	{
		if(type == ButtonType::ePause)
			setPaused(!getPaused());
	}
	else
	{
		if(pressedTabButton == type)
			return;

		if(pressedTabButton != ButtonType::COUNT)
			buttons[(size_t)pressedTabButton].setPressed(false);

		pressedTabButton = type;

		if(type == ButtonType::eMenu)
			gui.toggleMenu();
		else if(type == ButtonType::eDiscoveries)
			gui.toggleDiscoveries();
		else if(type == ButtonType::eDebug)
			gui.toggleDebugOptions();

		buttons[(size_t)type].setPressed(true);
	}


}
