module;

#include <SDL3/SDL_messagebox.h>

module Logger;

using namespace std::literals;

bool Logger::init()
{
	infoLog.open(Configuration::infoLogFileName.data() + ".txt"s, std::ios::binary | std::ios::out | std::ios::trunc);
	errorLog.open(Configuration::errorLogFileName.data() + ".txt"s, std::ios::binary | std::ios::out | std::ios::trunc);
	if(!infoLog || !errorLog)
	{
		displayErrorMessage("Couldn't create log file, check if game folder needs admin permissions");
		return false;
	}

	return true;
}

void Logger::logError(std::string_view message)
{
	auto stackTrace = std::stacktrace::current();
	std::println(errorLog, "Error: {}\nStacktrace:\n{}", message, stackTrace);
	errorLog.flush();

	if constexpr(isDebugBuild)
		std::println(std::cerr, "Error: {}\nStacktrace:\n{}", message, stackTrace);

	displayErrorMessage(message.data() + "\nCheck the error log for details. Esc to exit"s);
}

void Logger::logInfo(std::string_view message)
{
	std::println(infoLog, "{}", message);
	infoLog.flush();

	if constexpr(isDebugBuild)
		std::println(std::cout, "{}", message);
}

void Logger::displayErrorMessage(std::string_view message)
{
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Application Error", message.data(), nullptr);
}