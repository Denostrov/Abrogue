module;

#include "vulkan/vulkan.hpp"

export module Logger;

export import std;

//Class for logging errors and messages to files and console
export class Logger
{
public:
	Logger() = default;
	[[nodiscard]] bool openFiles();

	void logError(std::string_view message);
	void logInfo(std::string_view message);

	[[nodiscard]] bool checkSDLError(bool checkValue);

	template<class Value, class Result>
	bool checkVulkanError(Value& value, Result result, std::string_view successMessage, std::string_view errorMessage);
	bool checkVulkanError(vk::Result result, std::string_view successMessage, std::string_view errorMessage);

	void extraAssert(bool condition, std::string_view message);

private:
	void displayErrorMessage(std::string_view message) const;

	std::ofstream infoLog;
	std::ofstream errorLog;
};

template<class Value, class Result>
bool Logger::checkVulkanError(Value& value, Result result, std::string_view successMessage, std::string_view errorMessage)
{
	if(checkVulkanError(result.result, successMessage, errorMessage))
		return true;

	value = std::move(result.value);
	return false;
}

export inline Logger logger;