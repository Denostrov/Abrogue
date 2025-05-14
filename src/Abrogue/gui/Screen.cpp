module Screen;

import GameSystems;

void Screen::updateMouseMoved(std::uint32_t x, std::uint32_t y)
{
	//Check if cursor is hovering over a pressable button
	for(auto& button : pressableButtons)
	{
		if(!button.checkCollision(x, y))
			continue;

		//Cursor is hovering over a new button
		if(&button != hoveredButton)
		{
			if(hoveredButton)
				hoveredButton->setHovered(false);

			button.setHovered(true);
			hoveredButton = &button;
		}

		return;
	}

	//No button is hovered over
	if(!hoveredButton)
		return;

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