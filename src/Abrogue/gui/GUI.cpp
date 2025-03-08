import GUI;

void GUI::init()
{
	buttons[eStartGame].setPosition(120, 30);

	fpsLabel.setPosition(0, 0);
	healthLabel.setPosition(0, 1);
	hungerLabel.setPosition(7, 1);
}

void GUI::showStartMenu()
{
	buttons[eStartGame].setText("New game");

	healthLabel.clear();
	hungerLabel.clear();
}

void GUI::startGame()
{
	buttons[eStartGame].clear();

	healthLabel.setText("Health");
	hungerLabel.setText("Nutrition");
}

void GUI::setFPS(std::uint32_t fps)
{
	std::array<char, 16> fpsString{"FPS:"};
	std::to_chars(fpsString.data() + 4, fpsString.data() + 14, fps);
	fpsLabel.setText(fpsString.data());
}