export module Logger;

export import Configuration;

export class Logger
{
public:
	static bool init();

	static void logError(std::string_view message);
	static void logInfo(std::string_view message);

private:
	static void displayErrorMessage(std::string_view message);

	inline static std::ofstream infoLog;
	inline static std::ofstream errorLog;
};