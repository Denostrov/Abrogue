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

	window = SDL_CreateWindow(fullAppName.c_str(), Configuration::getWindowWidth(), Configuration::getWindowHeight(), SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);
	if(checkSDLErrorOccured(!window))
		return;

	uint32_t extensionCount{};
	auto extensionsArray = SDL_Vulkan_GetInstanceExtensions(&extensionCount);
	if(checkSDLErrorOccured(!extensionsArray))
		return;

	requiredExtensions.reserve(extensionCount);
	for(uint32_t i{}; i < extensionCount; i++)
		requiredExtensions.emplace_back(extensionsArray[i]);
}

RenderWindow::~RenderWindow()
{
	if(window)
		SDL_DestroyWindow(window);

	SDL_Quit();
}

VkSurfaceKHR RenderWindow::createSurface(VkInstance instance)
{
	VkSurfaceKHR result;
	if(checkSDLErrorOccured(!SDL_Vulkan_CreateSurface(window, instance, nullptr, &result)))
		return VkSurfaceKHR{};

	return result;
}

std::pair<uint32_t, uint32_t> RenderWindow::getFramebufferSize() const
{
	int width{}, height{};
	SDL_GetWindowSizeInPixels(window, &width, &height); 
	return {width, height};
}

bool RenderWindow::checkSDLErrorOccured(bool checkValue)
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
}
