module;

#include <json.hpp>

module Configuration;

import Constants;
import Logger;
import Random;
import GUI;
import RenderWindow;

bool Configuration::init()
{
	scancodeToInputControl.fill(InputControlType::COUNT);
	inputControlToScancode.fill(SDL_SCANCODE_UNKNOWN);

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

void Configuration::updateWindowOptions()
{
	isFullscreen = renderWindow.getIsFullscreen();
	if(isFullscreen)
		return;

	isMaximized = renderWindow.getIsMaximized();
	if(isMaximized)
		return;

	std::tie(windowWidth, windowHeight) = renderWindow.getWindowSize();
}

std::string_view Configuration::getInputControlName(InputControlType type) const
{
	auto scancode = inputControlToScancode[type];
	switch(scancode)
	{
		case SDL_SCANCODE_A: return "[A]"sv;
		case SDL_SCANCODE_B: return "[B]"sv;
		case SDL_SCANCODE_C: return "[C]"sv;
		case SDL_SCANCODE_D: return "[D]"sv;
		case SDL_SCANCODE_E: return "[E]"sv;
		case SDL_SCANCODE_F: return "[F]"sv;
		case SDL_SCANCODE_G: return "[G]"sv;
		case SDL_SCANCODE_H: return "[H]"sv;
		case SDL_SCANCODE_I: return "[I]"sv;
		case SDL_SCANCODE_J: return "[J]"sv;
		case SDL_SCANCODE_K: return "[K]"sv;
		case SDL_SCANCODE_L: return "[L]"sv;
		case SDL_SCANCODE_M: return "[M]"sv;
		case SDL_SCANCODE_N: return "[N]"sv;
		case SDL_SCANCODE_O: return "[O]"sv;
		case SDL_SCANCODE_P: return "[P]"sv;
		case SDL_SCANCODE_Q: return "[Q]"sv;
		case SDL_SCANCODE_R: return "[R]"sv;
		case SDL_SCANCODE_S: return "[S]"sv;
		case SDL_SCANCODE_T: return "[T]"sv;
		case SDL_SCANCODE_U: return "[U]"sv;
		case SDL_SCANCODE_V: return "[V]"sv;
		case SDL_SCANCODE_W: return "[W]"sv;
		case SDL_SCANCODE_X: return "[X]"sv;
		case SDL_SCANCODE_Y: return "[Y]"sv;
		case SDL_SCANCODE_Z: return "[Z]"sv;
		case SDL_SCANCODE_1: return "[1]"sv;
		case SDL_SCANCODE_2: return "[2]"sv;
		case SDL_SCANCODE_3: return "[3]"sv;
		case SDL_SCANCODE_4: return "[4]"sv;
		case SDL_SCANCODE_5: return "[5]"sv;
		case SDL_SCANCODE_6: return "[6]"sv;
		case SDL_SCANCODE_7: return "[7]"sv;
		case SDL_SCANCODE_8: return "[8]"sv;
		case SDL_SCANCODE_9: return "[9]"sv;
		case SDL_SCANCODE_0: return "[0]"sv;
		case SDL_SCANCODE_RETURN: return "[RET]"sv;
		case SDL_SCANCODE_ESCAPE: return "[ESC]"sv;
		case SDL_SCANCODE_BACKSPACE: return "[BSPACE]"sv;
		case SDL_SCANCODE_TAB: return "[TAB]"sv;
		case SDL_SCANCODE_SPACE: return "[SPACE]"sv;
		case SDL_SCANCODE_MINUS: return "[-]"sv;
		case SDL_SCANCODE_EQUALS: return "[=]"sv;
		case SDL_SCANCODE_LEFTBRACKET: return "[ [ ]"sv;
		case SDL_SCANCODE_RIGHTBRACKET: return "[ ] ]"sv;
		case SDL_SCANCODE_BACKSLASH: return "[\\]"sv;
		case SDL_SCANCODE_SEMICOLON: return "[;]"sv;
		case SDL_SCANCODE_APOSTROPHE: return "[']"sv;
		case SDL_SCANCODE_GRAVE: return "[~]"sv;
		case SDL_SCANCODE_COMMA: return "[,]"sv;
		case SDL_SCANCODE_PERIOD: return "[.]"sv;
		case SDL_SCANCODE_SLASH: return "[/]"sv;
		case SDL_SCANCODE_CAPSLOCK: return "[CAPS]"sv;
		case SDL_SCANCODE_F1: return "[F1]"sv;
		case SDL_SCANCODE_F2: return "[F2]"sv;
		case SDL_SCANCODE_F3: return "[F3]"sv;
		case SDL_SCANCODE_F4: return "[F4]"sv;
		case SDL_SCANCODE_F5: return "[F5]"sv;
		case SDL_SCANCODE_F6: return "[F6]"sv;
		case SDL_SCANCODE_F7: return "[F7]"sv;
		case SDL_SCANCODE_F8: return "[F8]"sv;
		case SDL_SCANCODE_F9: return "[F9]"sv;
		case SDL_SCANCODE_F10: return "[F10]"sv;
		case SDL_SCANCODE_F11: return "[F11]"sv;
		case SDL_SCANCODE_F12: return "[F12]"sv;
		case SDL_SCANCODE_F13: return "[F13]"sv;
		case SDL_SCANCODE_F14: return "[F14]"sv;
		case SDL_SCANCODE_F15: return "[F15]"sv;
		case SDL_SCANCODE_F16: return "[F16]"sv;
		case SDL_SCANCODE_F17: return "[F17]"sv;
		case SDL_SCANCODE_F18: return "[F18]"sv;
		case SDL_SCANCODE_F19: return "[F19]"sv;
		case SDL_SCANCODE_F20: return "[F20]"sv;
		case SDL_SCANCODE_F21: return "[F21]"sv;
		case SDL_SCANCODE_F22: return "[F22]"sv;
		case SDL_SCANCODE_F23: return "[F23]"sv;
		case SDL_SCANCODE_F24: return "[F24]"sv;
		case SDL_SCANCODE_PRINTSCREEN: return "[PRINT]"sv;
		case SDL_SCANCODE_SCROLLLOCK: return "[SCROLL]"sv;
		case SDL_SCANCODE_PAUSE: return "[PAUSE]"sv;
		case SDL_SCANCODE_INSERT: return "[INSERT]"sv;
		case SDL_SCANCODE_HOME: return "[HOME]"sv;
		case SDL_SCANCODE_PAGEUP: return "[PGUP]"sv;
		case SDL_SCANCODE_DELETE: return "[DELETE]"sv;
		case SDL_SCANCODE_END: return "[END]"sv;
		case SDL_SCANCODE_PAGEDOWN: return "[PGDOWN]"sv;
		case SDL_SCANCODE_RIGHT: return "[RIGHT]"sv;
		case SDL_SCANCODE_LEFT: return "[LEFT]"sv;
		case SDL_SCANCODE_DOWN: return "[DOWN]"sv;
		case SDL_SCANCODE_UP: return "[UP]"sv;
		case SDL_SCANCODE_NUMLOCKCLEAR: return "[NLOCK]"sv;
		case SDL_SCANCODE_KP_DIVIDE: return "[DIVIDE]"sv;
		case SDL_SCANCODE_KP_MULTIPLY: return "[MULT]"sv;
		case SDL_SCANCODE_KP_MINUS: return "[MINUS]"sv;
		case SDL_SCANCODE_KP_PLUS: return "[PLUS]"sv;
		case SDL_SCANCODE_KP_ENTER: return "[ENTER]"sv;
		case SDL_SCANCODE_KP_1: return "[Num1]"sv;
		case SDL_SCANCODE_KP_2: return "[Num2]"sv;
		case SDL_SCANCODE_KP_3: return "[Num3]"sv;
		case SDL_SCANCODE_KP_4: return "[Num4]"sv;
		case SDL_SCANCODE_KP_5: return "[Num5]"sv;
		case SDL_SCANCODE_KP_6: return "[Num6]"sv;
		case SDL_SCANCODE_KP_7: return "[Num7]"sv;
		case SDL_SCANCODE_KP_8: return "[Num8]"sv;
		case SDL_SCANCODE_KP_9: return "[Num9]"sv;
		case SDL_SCANCODE_KP_0: return "[Num0]"sv;
		case SDL_SCANCODE_KP_PERIOD: return "[PERIOD]"sv;
		case SDL_SCANCODE_KP_EQUALS: return "[EQUALS]"sv;
		case SDL_SCANCODE_NONUSBACKSLASH: return "[BSLASH]"sv;
		case SDL_SCANCODE_APPLICATION: return "[APP]"sv;
		case SDL_SCANCODE_POWER: return "[POWER]"sv;
		case SDL_SCANCODE_LCTRL: return "[LCTRL]"sv;
		case SDL_SCANCODE_LSHIFT: return "[LSHIFT]"sv;
		case SDL_SCANCODE_LALT: return "[LALT]"sv;
		case SDL_SCANCODE_LGUI: return "[LGUI]"sv;
		case SDL_SCANCODE_RCTRL: return "[RCTRL]"sv;
		case SDL_SCANCODE_RSHIFT: return "[RSHIFT]"sv;
		case SDL_SCANCODE_RALT: return "[RALT]"sv;
		case SDL_SCANCODE_RGUI: return "[RGUI]"sv;
		case (SDL_Scancode)301: return "[M1]"sv;
		case (SDL_Scancode)302: return "[M2]"sv;
		case (SDL_Scancode)303: return "[M3]"sv;
		case (SDL_Scancode)304: return "[M4]"sv;
		case (SDL_Scancode)305: return "[M5]"sv;
		case (SDL_Scancode)306: return "[M6]"sv;
		case (SDL_Scancode)307: return "[M7]"sv;
		case (SDL_Scancode)308: return "[M8]"sv;
		case (SDL_Scancode)309: return "[M9]"sv;
		case (SDL_Scancode)310: return "[M10]"sv;
		case (SDL_Scancode)311: return "[M11]"sv;
		case (SDL_Scancode)312: return "[M12]"sv;
		case (SDL_Scancode)313: return "[M13]"sv;
		case (SDL_Scancode)314: return "[M14]"sv;
		case (SDL_Scancode)315: return "[M15]"sv;
		case (SDL_Scancode)316: return "[M16]"sv;
		case SDL_SCANCODE_UNKNOWN: return "[NONE]"sv;
		default: return "[???]"sv;
	}
}

void Configuration::setInputControlScancode(InputControlType type, SDL_Scancode scancode)
{
	auto oldScancode = inputControlToScancode[type];
	if(oldScancode != SDL_SCANCODE_UNKNOWN)
		scancodeToInputControl[oldScancode] = InputControlType::COUNT;

	auto oldControl = scancodeToInputControl[scancode];
	if(oldControl != InputControlType::COUNT)
		inputControlToScancode[oldControl] = SDL_SCANCODE_UNKNOWN;

	scancodeToInputControl[scancode] = type;
	inputControlToScancode[type] = scancode;

	gui.refreshScreens();
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
	readJSONValue(configJSON, "windowFullscreen"sv, isFullscreen);
	readJSONValue(configJSON, "windowMaximized"sv, isMaximized);

	SDL_Scancode scancode{};
	readJSONValue(configJSON, "controlMoveUp"sv, scancode);
	setInputControlScancode(InputControlType::eMoveUp, scancode);
	readJSONValue(configJSON, "controlMoveDown"sv, scancode);
	setInputControlScancode(InputControlType::eMoveDown, scancode);
	readJSONValue(configJSON, "controlMoveLeft"sv, scancode);
	setInputControlScancode(InputControlType::eMoveLeft, scancode);
	readJSONValue(configJSON, "controlMoveRight"sv, scancode);
	setInputControlScancode(InputControlType::eMoveRight, scancode);
	readJSONValue(configJSON, "controlAttack"sv, scancode);
	setInputControlScancode(InputControlType::eAttack, scancode);
	readJSONValue(configJSON, "controlPause"sv, scancode);
	setInputControlScancode(InputControlType::ePause, scancode);
	readJSONValue(configJSON, "controlSearch"sv, scancode);
	setInputControlScancode(InputControlType::eSearch, scancode);
	readJSONValue(configJSON, "controlDiscoveries"sv, scancode);
	setInputControlScancode(InputControlType::eDiscoveries, scancode);
	readJSONValue(configJSON, "controlMenu"sv, scancode);
	setInputControlScancode(InputControlType::eMenu, scancode);
	readJSONValue(configJSON, "controlDebug"sv, scancode);
	setInputControlScancode(InputControlType::eDebug, scancode);
	readJSONValue(configJSON, "controlStopTime"sv, scancode);
	setInputControlScancode(InputControlType::eStopTime, scancode);
	readJSONValue(configJSON, "controlStepTime"sv, scancode);
	setInputControlScancode(InputControlType::eStepTime, scancode);

	return true;
}

bool Configuration::saveOptionsToFile()
{
	nlohmann::json configJSON;
	configJSON["windowWidth"sv] = windowWidth;
	configJSON["windowHeight"sv] = windowHeight;
	configJSON["windowFullscreen"sv] = isFullscreen;
	configJSON["windowMaximized"sv] = isMaximized;

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
