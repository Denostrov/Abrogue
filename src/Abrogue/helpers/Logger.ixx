export module Logger;

export import std;

export class Logger
{
public:
	void logError(std::string_view message);
	void logInfo(std::string_view message);

	void extraAssert(bool condition, std::string_view message);

private:
	bool openFiles();

	void displayErrorMessage(std::string_view message);

	std::ofstream infoLog;
	std::ofstream errorLog;

	friend class Game;
};