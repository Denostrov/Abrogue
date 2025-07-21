module PlayArea;

import Game;
import GUI;

using namespace std::literals;

void PlayArea::init()
{
	buttons[(size_t)ButtonType::ePause].init("Pause[SPACE]"sv, 26, 1, QuadPool::eMap);
	buttons[(size_t)ButtonType::eHealth].init("       Health       "sv, 0, 1, QuadPool::eMap);
	buttons[(size_t)ButtonType::eHealth].setBackgroundColor(Constants::healthBackgroundColor, Constants::healthHoverColor);
	buttons[(size_t)ButtonType::eNutrition].init("     Nutrition      "sv, 0, 2, QuadPool::eMap);
	buttons[(size_t)ButtonType::eNutrition].setBackgroundColor(Constants::nutritionBackgroundColor, Constants::nutritionHoverColor);
	buttons[(size_t)ButtonType::eGold].init("Gold:0"sv, 0, 5, QuadPool::eMap);
	buttons[(size_t)ButtonType::eSearch].init("Search[S]"sv, 11, 35, QuadPool::eMap);
	buttons[(size_t)ButtonType::eInventory].init("Inventory"sv, 0, 6, QuadPool::eMap);

	for(std::size_t i = 0; i < (std::size_t)ButtonType::eInventorySlotLast - (std::size_t)ButtonType::eInventorySlotFirst; i++)
		buttons[(std::size_t)ButtonType::eInventorySlotFirst + i].init(""sv, 0, 7 + i, QuadPool::eMap);

	buttons[(size_t)ButtonType::eDepth].init("Depth:"sv, 0, 35, QuadPool::eMap);

	buttons[(size_t)ButtonType::eDebug].init("Debug[F3]"sv, 11, 0, QuadPool::eMap);
	buttons[(size_t)ButtonType::eDiscoveries].init("Discoveries[D]"sv, 22, 35, QuadPool::eMap);
	buttons[(size_t)ButtonType::eMenu].init("Menu[ESC]"sv, 38, 35, QuadPool::eMap);

	pressableButtons = buttons;
}

void PlayArea::updateInventory(FixedVector<Item, 20> const& inventory, std::int64_t gold)
{
	std::array<char, 32> goldString{"Gold:"};
	std::to_chars(goldString.data() + 5, goldString.data() + 31, gold);
	buttons[(size_t)ButtonType::eGold].setText(goldString.data());

	for(std::size_t i = 0; i < inventory.size(); i++)
		buttons[(std::size_t)ButtonType::eInventorySlotFirst + i].setText(inventory[i].getName());

	for(std::size_t i = inventory.size(); i < 20; i++)
		buttons[(std::size_t)ButtonType::eInventorySlotFirst + i].setText(""sv);
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
	buttons[(size_t)ButtonType::ePause].setText(paused ? "PAUSED[SPACE]"sv : "Pause[SPACE]"sv);
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

void PlayArea::setPlayerHealth(double percentage)
{
	buttons[(size_t)ButtonType::eHealth].setProgress(percentage);
}

void PlayArea::onButtonPressed(size_t index)
{
	auto type = (ButtonType)index;

	//A normal button is pressed
	if(type < ButtonType::eDebug)
	{
		if(type == ButtonType::ePause)
			setPaused(!getPaused());

		return;
	}

	//A toggleable tab button is pressed
	if(pressedTabButton == type)
		return;

	if(pressedTabButton != ButtonType::COUNT)
		buttons[(size_t)pressedTabButton].setPressed(false);

	pressedTabButton = type;

	if(type == ButtonType::eMenu)
		gui.onPauseMenuHotkeyPressed();
	else if(type == ButtonType::eDiscoveries)
		gui.onDiscoveriesHotkeyPressed();
	else if(type == ButtonType::eDebug)
		gui.onDebugHotkeyPressed();

	buttons[(size_t)type].setPressed(true);
}
