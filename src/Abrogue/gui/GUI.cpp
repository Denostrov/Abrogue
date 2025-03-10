import GUI;

import GameSystems;

void GUI::init()
{
	buttons[eStartGame].setPosition(120, 30);
	buttons[eResume].setPosition(0, 2);
	buttons[eQuitToMenu].setPosition(0, 3);

	fpsLabel.setPosition(0, 0);
	healthLabel.setPosition(0, 1);
	hungerLabel.setPosition(7, 1);
}

void GUI::showStartMenu()
{
	buttons[eStartGame].setText("New game");
}

void GUI::startGame()
{
	buttons[eStartGame].clear();

	healthLabel.setText("Health");
	hungerLabel.setText("Nutrition");
}

void GUI::quitToMenu()
{
	buttons[eResume].clear();
	buttons[eQuitToMenu].clear();
	healthLabel.clear();
	hungerLabel.clear();

	buttons[eStartGame].setText("New game");
}

void GUI::pauseGame()
{
	buttons[eResume].setText("Resume");
	buttons[eQuitToMenu].setText("Quit to menu");
}

void GUI::resumeGame()
{
	buttons[eResume].clear();
	buttons[eQuitToMenu].clear();
}

void GUI::onMousePressed(std::uint32_t x, std::uint32_t y)
{
	for(size_t i = 0; i < buttons.size(); i++)
	{
		if(buttons[i].checkCollision(x, y))
			onButtonPressed((ButtonType)i);
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
	else if(type == eResume)
		game.resumeGame();
	else if(type == eQuitToMenu)
		game.quitToMenu();
}
