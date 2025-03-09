export module Configuration;

export import std;

export class Configuration
{
public:
	auto getWindowWidth() const { return windowWidth; }
	auto getWindowHeight() const { return windowHeight; }

private:
	bool load();
	bool saveToFile();

	std::uint32_t windowWidth{800};
	std::uint32_t windowHeight{450};

	friend class Game;
};
