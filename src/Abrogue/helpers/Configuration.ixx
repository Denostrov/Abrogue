module;

#include <json.hpp>
#include <SDL3/SDL_scancode.h>

export module Configuration;

export import Helpers;
export import FixedVector;
export import FixedString;
export import Logger;

using namespace std::literals;

//Enum for visual types of weapons
export enum class WeaponType
{
	eClaw,
	eClub,
	eDagger
};

//Enum for controls mapped to player input
export enum class InputControlType
{
	eMoveUp,
	eMoveDown,
	eMoveLeft,
	eMoveRight,
	eAttack,
	ePause,
	eSearch,
	eDiscoveries,
	eMenu,
	eDebug,
	eStopTime,
	eStepTime,
	COUNT
};

//Struct for defining an enemy type
export struct EnemyData
{
	FixedVector<char, 16> name;
	std::uint8_t symbol{};
	Color color{};
	double speed{10.0};
	double mass{10.0};
	WeaponType weaponType{};
	Color weaponColor{};
	std::int64_t damage{};
	double attackTime{0.25};
};

//Class for saving and loading configuration options
export class Configuration
{
public:
	Configuration() = default;
	[[nodiscard]] bool init();

	void saveOptions() { saveOptionsToFile(); }

	[[nodiscard]] auto getWindowWidth() const { return windowWidth; }
	[[nodiscard]] auto getWindowHeight() const { return windowHeight; }
	[[nodiscard]] auto getIsFullscreen() const { return isFullscreen; }
	[[nodiscard]] auto getIsMaximized() const { return isMaximized; }

	void updateWindowOptions();

	[[nodiscard]] InputControlType getInputControlFromScancode(SDL_Scancode scancode) const { return scancodeToInputControl[scancode]; }
	[[nodiscard]] SDL_Scancode getScancodeFromInputControl(InputControlType type) const { return inputControlToScancode[type]; }

	[[nodiscard]] std::string_view getInputControlName(InputControlType type) const;
	void setInputControlScancode(InputControlType type, SDL_Scancode scancode);
	void resetInputControlsToDefault();

	[[nodiscard]] optCRef<EnemyData> getSuitableEnemy();

private:
	nlohmann::json openJSONFile(std::string_view fileName);
	template<class Value>
	void readJSONValue(nlohmann::json const& json, std::string_view key, Value& value);

	bool loadOptions();
	bool saveOptionsToFile();
	void loadData();

	void setDefaultControls();

	std::int64_t windowWidth{800};
	std::int64_t windowHeight{450};
	bool isFullscreen{};
	bool isMaximized{};

	Array<InputControlType, SDL_Scancode::SDL_SCANCODE_COUNT> scancodeToInputControl;
	Array<SDL_Scancode, InputControlType::COUNT> inputControlToScancode;

	FixedVector<EnemyData, 128> enemyData;
};

export inline Configuration configuration;

template<class Value>
void Configuration::readJSONValue(nlohmann::json const& json, std::string_view key, Value& value)
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
	else if constexpr(std::is_same_v<ValueType, bool>)
	{
		if(!json[key].is_boolean())
		{
			logger.logInfo("Requested JSON value was not a boolean"sv);
			return;
		}

		value = json[key].get<bool>();
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
			return;
		}

		value = json[key].get<ValueType>();
	}
	else if constexpr(std::is_enum_v<ValueType>)
	{
		if(!json[key].is_number_unsigned())
		{
			logger.logInfo("Requested JSON value was not an enum"sv);
			return;
		}

		value = (ValueType)json[key].get<std::size_t>();
	}
	else
	{
		logger.logInfo("Requested JSON value of unknown type"sv);
	}
}
