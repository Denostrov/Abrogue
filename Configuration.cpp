module;

#include <json.hpp>

module Configuration;

import std;

void Configuration::init()
{
	auto configFile = std::ifstream("config.json", std::ios::in | std::ios::binary);
	if(!configFile)
	{
		saveToFile();
		return;
	}

	nlohmann::json configJSON = nlohmann::json::parse(configFile, nullptr, false);
	if(configJSON.is_discarded() || !configJSON.is_object())
	{
		saveToFile();
		return;
	}
		

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
}

bool Configuration::saveToFile()
{
	nlohmann::json configJSON;
	configJSON["windowWidth"] = windowWidth;
	configJSON["windowHeight"] = windowHeight;

	std::ofstream configFile("config.json", std::ios::out | std::ios::binary);
	if(!configFile)
		return false;

	configFile << std::setw(4) << configJSON << std::endl;
	return true;
}