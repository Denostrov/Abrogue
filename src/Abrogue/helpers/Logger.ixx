module;

#include "vulkan/vulkan.hpp"

export module Logger;

export import std;

export class Logger
{
public:
	//Open log files for writing
	[[nodiscard]] bool openFiles();

	//Show error message popup
	void logError(std::string_view message);
	//Print message to log
	void logInfo(std::string_view message);

	//Return true if an SDL error occured
	[[nodiscard]] bool checkSDLError(bool checkValue);

	//Return true if a Vulkan error occured
	template<class Value, class Result>
	bool checkVulkanError(Value& value, Result result, std::string_view successMessage, std::string_view errorMessage);
	bool checkVulkanError(vk::Result result, std::string_view successMessage, std::string_view errorMessage);

	//Show error popup and exit program
	void extraAssert(bool condition, std::string_view message);

private:
	//Show popup with error message
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