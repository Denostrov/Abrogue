module PlayArea;

import Game;
import GUI;

using namespace std::literals;

void PlayArea::init()
{
	using enum ButtonType;

	buttons[ePause].init("Pause[SPACE]"sv, 26, 1, QuadPool::eMap);
	buttons[eHealth].init("       Health       "sv, 0, 1, QuadPool::eMap);
	buttons[eHealth].setBackgroundColor(Constants::healthBackgroundColor, Constants::healthHoverColor);
	buttons[eNutrition].init("     Nutrition      "sv, 0, 2, QuadPool::eMap);
	buttons[eNutrition].setBackgroundColor(Constants::nutritionBackgroundColor, Constants::nutritionHoverColor);
	buttons[eGold].init("Gold:0"sv, 0, 5, QuadPool::eMap);
	buttons[eSearch].init("Search[S]"sv, 11, 35, QuadPool::eMap);
	buttons[eInventory].init("Inventory"sv, 0, 6, QuadPool::eMap);

	for(std::size_t i = 0; i < (std::size_t)eInventorySlotLast - (std::size_t)eInventorySlotFirst; i++)
		buttons[(std::size_t)eInventorySlotFirst + i].init(""sv, 0, 7 + i, QuadPool::eMap);

	buttons[eDepth].init("Depth:"sv, 0, 35, QuadPool::eMap);

	tabButtons[TabButtonType::eDebug].init("Debug[F3]"sv, 11, 0, QuadPool::eMap);
	tabButtons[TabButtonType::eDiscoveries].init("Discoveries[D]"sv, 22, 35, QuadPool::eMap);
	tabButtons[TabButtonType::eMenu].init("Menu[ESC]"sv, 38, 35, QuadPool::eMap);
}

void PlayArea::updateInventory(FixedVector<Item, 20> const& inventory, std::int64_t gold)
{
	using enum ButtonType;

	std::array<char, 32> goldString{"Gold:"};
	std::to_chars(goldString.data() + 5, goldString.data() + 31, gold);
	buttons[eGold].setText(goldString.data());

	for(std::size_t i = 0; i < inventory.size(); i++)
		buttons[(std::size_t)eInventorySlotFirst + i].setText(inventory[i].getName());

	for(std::size_t i = inventory.size(); i < 20; i++)
		buttons[(std::size_t)eInventorySlotFirst + i].setText(""sv);
}

void PlayArea::setPaused(bool paused)
{
	using enum ButtonType;

	if(buttons[ePause].getPressed() == paused)
		return;

	buttons[ePause].setPressed(paused);
	buttons[ePause].setText(paused ? "PAUSED[SPACE]"sv : "Pause[SPACE]"sv);
	game.setPaused(paused);
}

void PlayArea::setPlayerHealth(double percentage)
{
	buttons[ButtonType::eHealth].setProgress(percentage);
}

void PlayArea::onButtonPressed(ButtonType type)
{
	using enum ButtonType;

	if(type == ePause)
		setPaused(!getPaused());
}

void PlayArea::onTabButtonPressed(TabButtonType type)
{
	using enum TabButtonType;

	if(type == eMenu)
		gui.onPauseMenuHotkeyPressed();
	else if(type == eDiscoveries)
		gui.onDiscoveriesHotkeyPressed();
	else if(type == eDebug)
		gui.onDebugHotkeyPressed();
}
