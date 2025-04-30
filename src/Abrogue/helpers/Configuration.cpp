module;

#include <json.hpp>

module Configuration;

import GameSystems;

using namespace std::literals;

bool Configuration::load()
{
	auto openJSONFile = [](std::string_view fileName)
	{
		nlohmann::json result;

		auto configFile = std::ifstream(fileName.data() + ".json"s, std::ios::in | std::ios::binary);
		if(!configFile)
			return result;

		result = nlohmann::json::parse(configFile, nullptr, false);
		return result;
	};

	auto readJSONValue = [](nlohmann::json const& json, std::string_view key, auto& value)
	{
		if(!json.contains(key))
			return;

		using ValueType = std::decay_t<decltype(value)>;

		if constexpr(std::is_integral_v<ValueType>)
		{
			if(!json[key].is_number_integer())
				return;

			value = json[key].get<ValueType>();
		}
	};

	auto configJSON = openJSONFile(Constants::configFileName);
	if(configJSON.is_discarded() || !configJSON.is_object())
	{
		bool saveResult = saveToFile();
		if(!saveResult)
			return false;

		configJSON = openJSONFile(Constants::configFileName);
		if(configJSON.is_discarded() || !configJSON.is_object())
			return false;
	}

	readJSONValue(configJSON, "windowWidth", windowWidth);
	readJSONValue(configJSON, "windowHeight", windowHeight);

	auto dataJSON = openJSONFile(Constants::dataFileName);
	if(dataJSON.is_discarded() || !dataJSON.is_object())
		dataJSON = nlohmann::json::parse(R"({})", nullptr, false);

	return true;
}

bool Configuration::saveToFile()
{
	nlohmann::json configJSON;
	configJSON["windowWidth"] = windowWidth;
	configJSON["windowHeight"] = windowHeight;

	std::ofstream configFile(Constants::configFileName.data() + ".json"s, std::ios::out | std::ios::binary);
	if(!configFile)
	{
		logger.logError("Couldn't create config file, check if game folder needs admin permissions");
		return false;
	}

	configFile << std::setw(4) << configJSON << std::endl;
	return true;
}