module PlayArea;

import Game;
import GUI;

using namespace std::literals;

void PlayArea::init()
{
	using enum ButtonType;

	buttons[(size_t)ePause].init("Pause[SPACE]"sv, 26, 1, QuadPool::eMap);
	buttons[(size_t)eHealth].init("       Health       "sv, 0, 1, QuadPool::eMap);
	buttons[(size_t)eHealth].setBackgroundColor(Constants::healthBackgroundColor, Constants::healthHoverColor);
	buttons[(size_t)eNutrition].init("     Nutrition      "sv, 0, 2, QuadPool::eMap);
	buttons[(size_t)eNutrition].setBackgroundColor(Constants::nutritionBackgroundColor, Constants::nutritionHoverColor);
	buttons[(size_t)eGold].init("Gold:0"sv, 0, 5, QuadPool::eMap);
	buttons[(size_t)eSearch].init("Search[S]"sv, 11, 35, QuadPool::eMap);
	buttons[(size_t)eInventory].init("Inventory"sv, 0, 6, QuadPool::eMap);

	for(std::size_t i = 0; i < (std::size_t)eInventorySlotLast - (std::size_t)eInventorySlotFirst; i++)
		buttons[(std::size_t)eInventorySlotFirst + i].init(""sv, 0, 7 + i, QuadPool::eMap);

	buttons[(size_t)eDepth].init("Depth:"sv, 0, 35, QuadPool::eMap);

	tabButtons[(size_t)TabButtonType::eDebug].init("Debug[F3]"sv, 11, 0, QuadPool::eMap);
	tabButtons[(size_t)TabButtonType::eDiscoveries].init("Discoveries[D]"sv, 22, 35, QuadPool::eMap);
	tabButtons[(size_t)TabButtonType::eMenu].init("Menu[ESC]"sv, 38, 35, QuadPool::eMap);
}

void PlayArea::updateInventory(FixedVector<Item, 20> const& inventory, std::int64_t gold)
{
	using enum ButtonType;

	std::array<char, 32> goldString{"Gold:"};
	std::to_chars(goldString.data() + 5, goldString.data() + 31, gold);
	buttons[(size_t)eGold].setText(goldString.data());

	for(std::size_t i = 0; i < inventory.size(); i++)
		buttons[(std::size_t)eInventorySlotFirst + i].setText(inventory[i].getName());

	for(std::size_t i = inventory.size(); i < 20; i++)
		buttons[(std::size_t)eInventorySlotFirst + i].setText(""sv);
}

void PlayArea::setPaused(bool paused)
{
	using enum ButtonType;

	if(buttons[(size_t)ePause].getPressed() == paused)
		return;

	buttons[(size_t)ePause].setPressed(paused);
	buttons[(size_t)ePause].setText(paused ? "PAUSED[SPACE]"sv : "Pause[SPACE]"sv);
	game.setPaused(paused);
}

void PlayArea::setPlayerHealth(double percentage)
{
	buttons[(size_t)ButtonType::eHealth].setProgress(percentage);
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
