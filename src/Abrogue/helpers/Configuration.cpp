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

		auto const& jsonValue = json[key];

		using ValueType = std::decay_t<decltype(value)>;

		if constexpr(std::is_same_v<ValueType, std::uint8_t>)
		{
			if(!jsonValue.is_string())
				return;

			value = jsonValue.get<std::string>()[0];
		}
		else if constexpr(std::is_same_v<ValueType, std::string>)
		{
			if(!jsonValue.is_string())
				return;

			value = jsonValue.get<ValueType>();
		}
		else if constexpr(std::is_same_v<ValueType, Color>)
		{
			if(!jsonValue.is_array() || jsonValue.size() != 4)
				return;

			for(std::uint64_t i = 0; i < 4; i++)
			{
				if(!jsonValue[i].is_number_integer())
					return;
			}

			value.r = jsonValue[0].get<std::uint8_t>();
			value.g = jsonValue[1].get<std::uint8_t>();
			value.b = jsonValue[2].get<std::uint8_t>();
			value.a = jsonValue[3].get<std::uint8_t>();
		}
		else if constexpr(std::is_integral_v<ValueType>)
		{
			if(!json[key].is_number_integer())
				return;

			value = json[key].get<ValueType>();
		}
		else if constexpr(std::is_floating_point_v<ValueType>)
		{
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

	if(dataJSON.contains("enemies"))
	{
		for(auto const& enemyJSON : dataJSON["enemies"])
		{
			EnemyData data;
			readJSONValue(enemyJSON, "name", data.name);
			readJSONValue(enemyJSON, "symbol", data.symbol);
			readJSONValue(enemyJSON, "color", data.color);
			readJSONValue(enemyJSON, "speed", data.speed);
			readJSONValue(enemyJSON, "mass", data.mass);
			readJSONValue(enemyJSON, "weaponColor", data.weaponColor);
			readJSONValue(enemyJSON, "damage", data.damage);
			readJSONValue(enemyJSON, "attackTime", data.attackTime);

			std::string weaponType;
			readJSONValue(enemyJSON, "weaponType", weaponType);
			data.weaponType = weaponType == "dagger" ? WeaponType::eDagger : weaponType == "club" ? WeaponType::eClub : WeaponType::eClaw;

			enemyData.emplace_back(data);
		}
	}

	if(enemyData.empty())
		enemyData.emplace_back();

	return true;
}

EnemyData const& Configuration::getSuitableEnemy()
{
	return enemyData[std::random_device()() % enemyData.size()];
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