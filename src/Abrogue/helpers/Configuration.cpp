module;

#include <json.hpp>

module Configuration;

import Constants;
import Logger;
import Random;

using namespace std::literals;

template<class Value>
void readJSONValue(nlohmann::json const& json, std::string_view key, Value& value)
{
	if(!json.contains(key))
	{
		logger.logInfo("Requested key not found in JSON"sv);
		return;
	}

	auto const& jsonValue = json[key];

	using ValueType = std::decay_t<decltype(value)>;

	if constexpr(std::is_same_v<ValueType, std::uint8_t>)
	{
		if(!jsonValue.is_string() || jsonValue.size() != 1)
		{
			logger.logInfo("Requested JSON value was not a char"sv);
			return;
		}

		value = jsonValue.get<std::string>()[0];
	}
	else if constexpr(IsCharArray<ValueType>)
	{
		if(!jsonValue.is_string())
		{
			logger.logInfo("Requested JSON value was not a string"sv);
			return;
		}

		auto str = jsonValue.get<std::string>();
		if(str.size() > value.capacity())
		{
			logger.logInfo("Requested JSON string is too big for storage"sv);
			str.resize(value.capacity());
		}

		value = str;
	}
	else if constexpr(std::is_same_v<ValueType, Color>)
	{
		if(!jsonValue.is_array() || jsonValue.size() != 4)
		{
			logger.logInfo("Requested JSON value was not a color array"sv);
			return;
		}

		for(std::uint64_t i = 0; i < 4; i++)
		{
			if(!jsonValue[i].is_number_integer())
			{
				logger.logInfo("Requested JSON value inside a color array was not an integer"sv);
				return;
			}
		}

		value.r = jsonValue[0].get<std::uint8_t>();
		value.g = jsonValue[1].get<std::uint8_t>();
		value.b = jsonValue[2].get<std::uint8_t>();
		value.a = jsonValue[3].get<std::uint8_t>();
	}
	else if constexpr(std::is_integral_v<ValueType>)
	{
		if(!json[key].is_number_integer())
		{
			logger.logInfo("Requested JSON value was not an integer"sv);
			return;
		}

		value = json[key].get<ValueType>();
	}
	else if constexpr(std::is_floating_point_v<ValueType>)
	{
		if(!json[key].is_number())
		{
			logger.logInfo("Requested JSON value was not a number"sv);
		}

		value = json[key].get<ValueType>();
	}
	else
	{
		logger.logInfo("Requested JSON value of unknown type"sv);
	}
}


bool Configuration::load()
{
	auto openJSONFile = [](std::string_view fileName)
	{
		nlohmann::json result;

		auto configFile = std::ifstream(fileName.data(), std::ios::in | std::ios::binary);
		if(!configFile)
			return result;

		result = nlohmann::json::parse(configFile, nullptr, false);
		return result;
	};

	auto configJSON = openJSONFile(Constants::configFileName);
	if(configJSON.is_discarded() || !configJSON.is_object())
	{
		bool saveResult = saveOptionsToFile();
		if(!saveResult)
			return false;

		configJSON = openJSONFile(Constants::configFileName);
		if(configJSON.is_discarded() || !configJSON.is_object())
			return false;
	}

	readJSONValue(configJSON, "windowWidth"sv, windowWidth);
	readJSONValue(configJSON, "windowHeight"sv, windowHeight);

	auto dataJSON = openJSONFile(Constants::dataFileName);
	if(dataJSON.is_discarded() || !dataJSON.is_object())
		dataJSON = nlohmann::json::parse(R"({})", nullptr, false);

	if(dataJSON.contains("enemies") && dataJSON["enemies"].is_array())
	{
		auto const& enemyArrayJSON = dataJSON["enemies"];
		if(enemyArrayJSON.size() > enemyData.capacity())
			logger.logInfo("Too many enemy types in config"sv);

		for(std::uint64_t i = 0; i < std::min(enemyArrayJSON.size(), enemyData.capacity()); i++)
		{
			auto const& enemyJSON = enemyArrayJSON[i];

			EnemyData data;
			readJSONValue(enemyJSON, "name"sv, data.name);
			readJSONValue(enemyJSON, "symbol"sv, data.symbol);
			readJSONValue(enemyJSON, "color"sv, data.color);
			readJSONValue(enemyJSON, "speed"sv, data.speed);
			readJSONValue(enemyJSON, "mass"sv, data.mass);
			readJSONValue(enemyJSON, "weaponColor"sv, data.weaponColor);
			readJSONValue(enemyJSON, "damage"sv, data.damage);
			readJSONValue(enemyJSON, "attackTime"sv, data.attackTime);

			FixedVector<char, 16> weaponType;
			readJSONValue(enemyJSON, "weaponType"sv, weaponType);
			data.weaponType = weaponType == "dagger"sv ? WeaponType::eDagger : weaponType == "club"sv ? WeaponType::eClub : WeaponType::eClaw;

			enemyData.emplace_back(data);
		}
	}

	return true;
}

optCRef<EnemyData> Configuration::getSuitableEnemy()
{
	return enemyData[mapRandom.generate() % enemyData.size()];
}

bool Configuration::saveOptionsToFile()
{
	nlohmann::json configJSON;
	configJSON["windowWidth"] = windowWidth;
	configJSON["windowHeight"] = windowHeight;

	std::ofstream configFile(Constants::configFileName.data(), std::ios::out | std::ios::binary);
	if(!configFile)
	{
		logger.logError("Couldn't create config file, check if game folder needs admin permissions"sv);
		return false;
	}

	configFile << std::setw(4) << configJSON << std::endl;
	return true;
}