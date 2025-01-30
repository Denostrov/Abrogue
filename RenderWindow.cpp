module;

#include <SDL3/SDL_messagebox.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_vulkan.h>

module RenderWindow;

import Configuration;

RenderWindow::RenderWindow()
{
	auto checkErrorOccured = [this](bool checkValue, std::string_view errorStr)
	{
		if(!checkValue)
			return false;

		if(SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Application Error", errorStr.data(), nullptr))
		{
			hasError = true;
			return true;
		}

		std::println(std::cerr, "{}", errorStr);
		return true;
	};
	auto checkSDLErrorOccured = [this](bool checkValue)
	{
		if(!checkValue)
			return false;

		std::string errorString = SDL_GetError();
		SDL_ClearError();

		if(errorString.empty())
		{
			hasError = true;
			return true;
		}

		if(SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "SDL Error", errorString.c_str(), nullptr))
		{
			hasError = true;
			return true;
		}

		std::println(std::cerr, "{}", errorString);
		return true;
	};

	auto fullAppName = Configuration::getAppName() + " " + Configuration::getAppVersion();
	if(checkSDLErrorOccured(!SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_NAME_STRING, fullAppName.c_str())))
		return;

	if(checkSDLErrorOccured(!SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_VERSION_STRING, Configuration::getAppVersion().c_str())))
		return;

	if(checkSDLErrorOccured(!SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_IDENTIFIER_STRING, Configuration::getAppIdentifier().c_str())))
		return;

	if(checkSDLErrorOccured(!SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_CREATOR_STRING, Configuration::getAppCreator().c_str())))
		return;

	if(checkSDLErrorOccured(!SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_COPYRIGHT_STRING, Configuration::getAppCopyright().c_str())))
		return;

	if(checkSDLErrorOccured(!SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_URL_STRING, Configuration::getAppURL().c_str())))
		return;

	if(checkSDLErrorOccured(!SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_TYPE_STRING, Configuration::getAppType().c_str())))
		return;

	if(checkSDLErrorOccured(!SDL_Init(SDL_INIT_VIDEO)))
		return;

	window = SDL_CreateWindow(fullAppName.c_str(), Configuration::getWindowWidth(), Configuration::getWindowHeight(), SDL_WINDOW_VULKAN);
	if(checkSDLErrorOccured(!window))
		return;

	uint32_t instanceCount{};
	auto extensions = SDL_Vulkan_GetInstanceExtensions(&instanceCount);
	if(checkSDLErrorOccured(!extensions))
		return;

	std::println(std::cerr, "{} vulkan instance extensions required:", instanceCount);
	for(uint32_t i{}; i < instanceCount; i++)
		std::println(std::cerr, "\t{}", extensions[i]);
}

RenderWindow::~RenderWindow()
{
	if(window)
		SDL_DestroyWindow(window);

	SDL_Quit();
}
