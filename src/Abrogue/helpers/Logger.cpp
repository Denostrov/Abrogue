module;

#include <SDL3/SDL_messagebox.h>
#include "vulkan/vulkan.hpp"

module Logger;

import Constants;
import FixedVector;

using namespace std::literals;

bool Logger::openFiles()
{
	infoLog.open(Constants::infoLogFileName.data(), std::ios::binary | std::ios::out | std::ios::trunc);
	errorLog.open(Constants::errorLogFileName.data(), std::ios::binary | std::ios::out | std::ios::trunc);
	if(!infoLog || !errorLog)
	{
		displayErrorMessage("Couldn't create log file, check if game folder needs admin permissions"sv);
		return false;
	}

	return true;
}

void Logger::logError(std::string_view message)
{
	auto stackTrace = std::stacktrace::current();
	std::println(errorLog, "Error: {}\nStacktrace:\n{}"sv, message, stackTrace);
	errorLog.flush();

	if constexpr(isDebugBuild)
		std::println(std::cerr, "Error: {}\nStacktrace:\n{}"sv, message, stackTrace);

	FixedVector<char, 512> popupMessage(message);
	popupMessage.append("\nCheck the error log for details. Esc to exit"sv);
	displayErrorMessage(popupMessage);
}

void Logger::logInfo(std::string_view message)
{
	std::println(infoLog, "{}", message);
	infoLog.flush();

	if constexpr(isDebugBuild)
		std::println(std::cout, "{}", message);
}

bool Logger::checkSDLError(bool checkValue)
{
	if(checkValue)
		return false;

	auto errorString = SDL_GetError();
	if(errorString[0] != '\0')
	{
		logError(errorString);
		SDL_ClearError();
	}

	return true;
}

bool Logger::checkVulkanError(vk::Result result, std::string_view successMessage, std::string_view errorMessage)
{
	if(result != vk::Result::eSuccess)
	{
		FixedVector<char, 1024> errorString(errorMessage);
		errorString.append(": "sv);
		errorString.append(vk::to_string(result));
		logError(errorString);
		return true;
	}
	else if(!successMessage.empty())
		logInfo(successMessage);

	return false;
}

void Logger::extraAssert(bool condition, std::string_view message)
{
	if constexpr(useExtraAsserts)
	{
		if(condition)
			return;

		logError(message);
		std::exit(1);
	}
}

void Logger::displayErrorMessage(std::string_view message) const
{
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Application Error", message.data(), nullptr);
}