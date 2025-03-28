module Screen;

import GameSystems;

void Screen::updateMouseMoved(std::uint32_t x, std::uint32_t y)
{
	for(auto& button : pressableButtons)
	{
		if(!button.checkCollision(x, y))
			continue;

		if(&button != hoveredButton)
		{
			if(hoveredButton)
				hoveredButton->setHovered(false);

			button.setHovered(true);
			hoveredButton = &button;
		}
		return;
	}

	if(hoveredButton)
		hoveredButton->setHovered(false);
	hoveredButton = nullptr;
}

void Screen::updateMousePressed(std::uint32_t x, std::uint32_t y)
{
	for(size_t i = 0; i < pressableButtons.size(); i++)
	{
		if(!pressableButtons[i].checkCollision(x, y))
			continue;

		onButtonPressed(i);
		return;
	}
}