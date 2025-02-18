module;

#include <json.hpp>

module Configuration;

import Logger;
import std;

using namespace std::literals;

bool Configuration::init()
{
	auto configFile = std::ifstream(configFileName.data() + ".json"s, std::ios::in | std::ios::binary);
	if(!configFile)
		return saveToFile();

	nlohmann::json configJSON = nlohmann::json::parse(configFile, nullptr, false);
	if(configJSON.is_discarded() || !configJSON.is_object())
		return saveToFile();

	auto readJSONValue = [&configJSON](std::string_view key, auto& value)
	{
		if(!configJSON.contains(key))
			return;

		using ValueType = std::decay_t<decltype(value)>;

		if constexpr(std::is_integral_v<ValueType>)
		{
			if(!configJSON[key].is_number_integer())
				return;

			value = configJSON[key].get<ValueType>();
		}
	};

	readJSONValue("windowWidth", windowWidth);
	readJSONValue("windowHeight", windowHeight);

	return true;
}

bool Configuration::saveToFile()
{
	nlohmann::json configJSON;
	configJSON["windowWidth"] = windowWidth;
	configJSON["windowHeight"] = windowHeight;

	std::ofstream configFile(configFileName.data() + ".json"s, std::ios::out | std::ios::binary);
	if(!configFile)
	{
		Logger::logError("Couldn't create config file, check if game folder needs admin permissions");
		return false;
	}

	configFile << std::setw(4) << configJSON << std::endl;
	return true;
}