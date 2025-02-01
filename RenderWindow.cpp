module;

#include <SDL3/SDL_messagebox.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_vulkan.h>

module RenderWindow;

import Logger;

using namespace std::literals;

RenderWindow::RenderWindow()
{
	auto checkErrorOccured = [this](bool checkValue, std::string_view errorStr)
	{
		if(!checkValue)
			return false;

		hasError = true;
		Logger::logError(errorStr);
		return true;
	};
	auto checkSDLErrorOccured = [this](bool checkValue)
	{
		if(!checkValue)
			return false;

		hasError = true;

		std::string errorString = SDL_GetError();
		SDL_ClearError();
		if(errorString.empty())
			return true;

		Logger::logError(errorString);
		return true;
	};

	auto fullAppName = Configuration::appName.data() + " "s + Configuration::appVersion.data();
	if(checkSDLErrorOccured(!SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_NAME_STRING, fullAppName.c_str())))
		return;

	if(checkSDLErrorOccured(!SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_VERSION_STRING, Configuration::appVersion.data())))
		return;

	if(checkSDLErrorOccured(!SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_IDENTIFIER_STRING, Configuration::appIdentifier.data())))
		return;

	if(checkSDLErrorOccured(!SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_CREATOR_STRING, Configuration::appCreator.data())))
		return;

	if(checkSDLErrorOccured(!SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_COPYRIGHT_STRING, Configuration::appCopyright.data())))
		return;

	if(checkSDLErrorOccured(!SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_URL_STRING, Configuration::appURL.data())))
		return;

	if(checkSDLErrorOccured(!SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_TYPE_STRING, Configuration::appType.data())))
		return;

	if(checkSDLErrorOccured(!SDL_Init(SDL_INIT_VIDEO)))
		return;

	window = SDL_CreateWindow(fullAppName.c_str(), Configuration::getWindowWidth(), Configuration::getWindowHeight(), SDL_WINDOW_VULKAN);
	if(checkSDLErrorOccured(!window))
		return;

	uint32_t instanceCount{};
	auto extensionsArray = SDL_Vulkan_GetInstanceExtensions(&instanceCount);
	if(checkSDLErrorOccured(!extensionsArray))
		return;

	requiredExtensions.reserve(instanceCount);
	for(uint32_t i{}; i < instanceCount; i++)
		requiredExtensions.emplace_back(extensionsArray[i]);
}

RenderWindow::~RenderWindow()
{
	if(window)
		SDL_DestroyWindow(window);

	SDL_Quit();
}
