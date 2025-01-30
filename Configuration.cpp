module;

#include <json.hpp>

module Configuration;

import std;

bool Configuration::init()
{
	nlohmann::json configJSON = nlohmann::json::parse(std::ifstream("config.json"), nullptr, false);
	if(configJSON.is_discarded() || !configJSON.is_object())
		return false;

	auto readJSONValue = [&configJSON](std::string_view key, auto& value)
	{
		if(!configJSON.contains(key))
			return;

		if constexpr(std::is_integral_v<decltype(value)>)
		{
			if(!configJSON[key.data()].is_number_integer())
				return;

			value = configJSON[key.data()].get<decltype(value)>();
		}
		
	};

	readJSONValue("windowWidth", windowWidth);
	readJSONValue("windowHeight", windowHeight);

	return true;
}

void Configuration::release()
{

}