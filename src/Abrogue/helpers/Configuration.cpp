module;

#include <json.hpp>

module Configuration;

import Constants;
import Logger;
import Random;

bool Configuration::load()
{
	if(!loadOptions())
		return false;

	loadData();

	return true;
}

optCRef<EnemyData> Configuration::getSuitableEnemy()
{
	return enemyData[mapRandom.generate() % enemyData.size()];
}

nlohmann::json Configuration::openJSONFile(std::string_view fileName)
{
	nlohmann::json result;

	auto configFile = std::ifstream(fileName.data(), std::ios::in | std::ios::binary);
	if(!configFile)
		return result;

	result = nlohmann::json::parse(configFile, nullptr, false);
	return result;
}

bool Configuration::loadOptions()
{
	//Try opening configuration file
	auto configJSON = openJSONFile(Constants::configFileName);
	if(configJSON.is_discarded() || !configJSON.is_object())
	{
		//If couldn't open, create a new one with default values
		bool saveResult = saveOptionsToFile();
		if(!saveResult)
			return false;

		//If still can't open, give up
		configJSON = openJSONFile(Constants::configFileName);
		if(configJSON.is_discarded() || !configJSON.is_object())
		{
			logger.logError("Couldn't open created config file, check if game folder needs admin permissions"sv);
			return false;
		}
	}

	readJSONValue(configJSON, "windowWidth"sv, windowWidth);
	readJSONValue(configJSON, "windowHeight"sv, windowHeight);
	return true;
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

void Configuration::loadData()
{
	auto dataJSON = openJSONFile(Constants::dataFileName);
	if(dataJSON.is_discarded() || !dataJSON.is_object())
		return;

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
}
