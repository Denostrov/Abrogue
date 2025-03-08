import GUI;

void GUI::init()
{
	startGameButton = Button("New game", ButtonPressedEvent::eStartGame, 120, 30);
}

void GUI::startGame()
{
	fpsLabel = Label("FPS:", 0, 0);
	healthLabel = Label("Health", 0, 1);
	hungerLabel = Label("Nutrition", 7, 1);
}

void GUI::setFPS(std::uint32_t fps)
{
	std::array<char, 16> fpsString{"FPS:"};
	std::to_chars(fpsString.data() + 4, fpsString.data() + 14, fps);
	fpsLabel.setText(fpsString.data());
}