module;

#include <json.hpp>

module Configuration;

import Constants;
import Logger;
import Random;

bool Configuration::init()
{
	scancodeToInputControl.fill(InputControlType::COUNT);
	inputControlToScancode.fill(SDL_SCANCODE_COUNT);

	setInputControlScancode(InputControlType::eMoveUp, SDL_SCANCODE_W);
	setInputControlScancode(InputControlType::eMoveDown, SDL_SCANCODE_S);
	setInputControlScancode(InputControlType::eMoveLeft, SDL_SCANCODE_A);
	setInputControlScancode(InputControlType::eMoveRight, SDL_SCANCODE_D);
	setInputControlScancode(InputControlType::eAttack, (SDL_Scancode)301);
	setInputControlScancode(InputControlType::ePause, SDL_SCANCODE_SPACE);
	setInputControlScancode(InputControlType::eSearch, SDL_SCANCODE_Z);
	setInputControlScancode(InputControlType::eDiscoveries, SDL_SCANCODE_C);
	setInputControlScancode(InputControlType::eMenu, SDL_SCANCODE_ESCAPE);
	setInputControlScancode(InputControlType::eDebug, SDL_SCANCODE_F3);
	setInputControlScancode(InputControlType::eStopTime, SDL_SCANCODE_KP_7);
	setInputControlScancode(InputControlType::eStepTime, SDL_SCANCODE_KP_8);

	if(!loadOptions())
		return false;

	loadData();

	if(!saveOptionsToFile())
		return false;

	return true;
}

FixedVector<char, 8> Configuration::getInputControlName(InputControlType type) const
{
	auto scancode = inputControlToScancode[type];
	switch(scancode)
	{
		case SDL_SCANCODE_W: return "[W]"sv;
		case SDL_SCANCODE_S: return "[S]"sv;
		case SDL_SCANCODE_A: return "[A]"sv;
		case SDL_SCANCODE_D: return "[D]"sv;
		case SDL_SCANCODE_Z: return "[Z]"sv;
		case SDL_SCANCODE_C: return "[C]"sv;
		case SDL_SCANCODE_SPACE: return "[SPACE]"sv;
		case (SDL_Scancode)301: return "[M1]"sv;
		case SDL_SCANCODE_ESCAPE: return "[ESC]"sv;
		case SDL_SCANCODE_F3: return "[F3]"sv;
		case SDL_SCANCODE_KP_7: return "[Num7]"sv;
		case SDL_SCANCODE_KP_8: return "[Num8]"sv;
		default: return "[???]"sv;
	}
}

void Configuration::setInputControlScancode(InputControlType type, SDL_Scancode scancode)
{
	scancodeToInputControl[scancode] = type;
	inputControlToScancode[type] = scancode;
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

	readJSONValue(configJSON, "controlMoveUp"sv, inputControlToScancode[InputControlType::eMoveUp]);
	readJSONValue(configJSON, "controlMoveDown"sv, inputControlToScancode[InputControlType::eMoveDown]);
	readJSONValue(configJSON, "controlMoveLeft"sv, inputControlToScancode[InputControlType::eMoveLeft]);
	readJSONValue(configJSON, "controlMoveRight"sv, inputControlToScancode[InputControlType::eMoveRight]);
	readJSONValue(configJSON, "controlAttack"sv, inputControlToScancode[InputControlType::eAttack]);
	readJSONValue(configJSON, "controlPause"sv, inputControlToScancode[InputControlType::ePause]);
	readJSONValue(configJSON, "controlSearch"sv, inputControlToScancode[InputControlType::eSearch]);
	readJSONValue(configJSON, "controlDiscoveries"sv, inputControlToScancode[InputControlType::eDiscoveries]);
	readJSONValue(configJSON, "controlMenu"sv, inputControlToScancode[InputControlType::eMenu]);
	readJSONValue(configJSON, "controlDebug"sv, inputControlToScancode[InputControlType::eDebug]);
	readJSONValue(configJSON, "controlStopTime"sv, inputControlToScancode[InputControlType::eStopTime]);
	readJSONValue(configJSON, "controlStepTime"sv, inputControlToScancode[InputControlType::eStepTime]);

	return true;
}

bool Configuration::saveOptionsToFile()
{
	nlohmann::json configJSON;
	configJSON["windowWidth"sv] = windowWidth;
	configJSON["windowHeight"sv] = windowHeight;

	configJSON["controlMoveUp"sv] = inputControlToScancode[InputControlType::eMoveUp];
	configJSON["controlMoveDown"sv] = inputControlToScancode[InputControlType::eMoveDown];
	configJSON["controlMoveLeft"sv] = inputControlToScancode[InputControlType::eMoveLeft];
	configJSON["controlMoveRight"sv] = inputControlToScancode[InputControlType::eMoveRight];
	configJSON["controlAttack"sv] = inputControlToScancode[InputControlType::eAttack];
	configJSON["controlPause"sv] = inputControlToScancode[InputControlType::ePause];
	configJSON["controlSearch"sv] = inputControlToScancode[InputControlType::eSearch];
	configJSON["controlDiscoveries"sv] = inputControlToScancode[InputControlType::eDiscoveries];
	configJSON["controlMenu"sv] = inputControlToScancode[InputControlType::eMenu];
	configJSON["controlDebug"sv] = inputControlToScancode[InputControlType::eDebug];
	configJSON["controlStopTime"sv] = inputControlToScancode[InputControlType::eStopTime];
	configJSON["controlStepTime"sv] = inputControlToScancode[InputControlType::eStepTime];

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
