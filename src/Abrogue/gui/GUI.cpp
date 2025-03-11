import GUI;

import GameSystems;

void GUI::init()
{
	buttons[eStartGame].init("New game", 120, 30);
	buttons[eQuitToDesktop].init("Quit to desktop", 113, 31);
	buttons[eResume].init("Resume", 0, 2);
	buttons[eQuitToMenu].init("Quit to menu", 0, 3);

	fpsLabel.init("FPS:", 0, 0, true);
	healthLabel.init("Health", 0, 1);
	healthLabel.setBackgroundColor(Constants::healthBackgroundColor, Constants::healthHoverColor);
	hungerLabel.init("Nutrition", 7, 1);
	hungerLabel.setBackgroundColor(Constants::nutritionBackgroundColor, Constants::nutritionHoverColor);
}

void GUI::showStartMenu()
{
	buttons[eStartGame].setVisible(true);
	buttons[eQuitToDesktop].setVisible(true);
}

void GUI::startGame()
{
	buttons[eStartGame].setVisible(false);
	buttons[eQuitToDesktop].setVisible(false);

	healthLabel.setVisible(true);
	hungerLabel.setVisible(true);
}

void GUI::quitToMenu()
{
	buttons[eResume].setVisible(false);
	buttons[eQuitToMenu].setVisible(false);
	healthLabel.setVisible(false);
	hungerLabel.setVisible(false);

	buttons[eStartGame].setVisible(true);
	buttons[eQuitToDesktop].setVisible(true);
}

void GUI::pauseGame()
{
	buttons[eResume].setVisible(true);
	buttons[eQuitToMenu].setVisible(true);
}

void GUI::resumeGame()
{
	buttons[eResume].setVisible(false);
	buttons[eQuitToMenu].setVisible(false);
}

void GUI::onMouseMoved(std::uint32_t x, std::uint32_t y)
{
	ButtonType newHoveredButton{COUNT};
	for(size_t i = 0; i < buttons.size(); i++)
	{
		if(buttons[i].checkCollision(x, y))
		{
			buttons[i].setHovered(true);
			newHoveredButton = (ButtonType)i;
			break;
		}
	}

	if(newHoveredButton != hoveredButton)
	{
		if(hoveredButton != COUNT)
			buttons[hoveredButton].setHovered(false);
		hoveredButton = newHoveredButton;
	}
}

void GUI::onMousePressed(std::uint32_t x, std::uint32_t y)
{
	for(size_t i = 0; i < buttons.size(); i++)
	{
		if(buttons[i].checkCollision(x, y))
		{
			onButtonPressed((ButtonType)i);
			break;
		}
	}
}

void GUI::setFPS(std::uint32_t fps)
{
	std::array<char, 16> fpsString{"FPS:"};
	std::to_chars(fpsString.data() + 4, fpsString.data() + 14, fps);
	fpsLabel.setText(fpsString.data());
}

void GUI::onButtonPressed(ButtonType type)
{
	if(type == eStartGame)
		game.startGame();
	else if(type == eQuitToDesktop)
		game.quitToDesktop();
	else if(type == eResume)
		game.resumeGame();
	else if(type == eQuitToMenu)
		game.quitToMenu();
}
