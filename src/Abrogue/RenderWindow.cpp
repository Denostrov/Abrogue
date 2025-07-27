module;

#include <SDL3/SDL_messagebox.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_vulkan.h>

module RenderWindow;

import Constants;
import Logger;
import Configuration;

using namespace std::literals;

bool RenderWindow::initSDL()
{
	//Set application metadata
	auto fullAppName = Constants::appName.data() + " "s + Constants::appVersion.data();
	if(logger.checkSDLError(SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_NAME_STRING, fullAppName.c_str()) && 
							SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_VERSION_STRING, Constants::appVersion.data()) && 
							SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_IDENTIFIER_STRING, Constants::appIdentifier.data()) &&
							SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_CREATOR_STRING, Constants::appCreator.data()) &&
							SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_COPYRIGHT_STRING, Constants::appCopyright.data()) &&
							SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_TYPE_STRING, Constants::appType.data())))
		return false;

	//Initialize SDL video system
	if(logger.checkSDLError(SDL_Init(SDL_INIT_VIDEO)))
		return false;

	int displayCount{};
	auto displays = SDL_GetDisplays(&displayCount);
	if(logger.checkSDLError(displays))
		return false;

	SDL_Rect displayUsableRect{};
	if(logger.checkSDLError(SDL_GetDisplayUsableBounds(displays[0], &displayUsableRect)))
		return false;
	SDL_free(displays);

	auto windowWidth = std::min((std::int64_t)displayUsableRect.w, configuration.getWindowWidth());
	auto windowHeight = std::min((std::int64_t)displayUsableRect.h, configuration.getWindowHeight());

	//Create window
	window = SDL_CreateWindow(fullAppName.c_str(), windowWidth, windowHeight,
							  SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE | (configuration.getIsFullscreen() ? SDL_WINDOW_FULLSCREEN : configuration.getIsMaximized() ? SDL_WINDOW_MAXIMIZED : 0));
	if(logger.checkSDLError(window))
		return false;

	auto usableCenterX = displayUsableRect.x + displayUsableRect.w / 2;
	auto usableCenterY = displayUsableRect.y + displayUsableRect.h / 2;
	if(logger.checkSDLError(SDL_SetWindowPosition(window, usableCenterX - windowWidth / 2, usableCenterY - windowHeight / 2)))
		return false;
	if(logger.checkSDLError(SDL_SyncWindow(window)))
	   return false;

	//Get required extensions
	uint32_t extensionCount{};
	auto extensionsArray = SDL_Vulkan_GetInstanceExtensions(&extensionCount);
	if(logger.checkSDLError(extensionsArray))
		return false;

	requiredExtensions.reserve(extensionCount);
	for(uint32_t i{}; i < extensionCount; i++)
		requiredExtensions.emplace_back(extensionsArray[i]);

	return true;
}

RenderWindow::~RenderWindow()
{
	if(window)
		SDL_DestroyWindow(window);

	if(SDL_WasInit(SDL_INIT_VIDEO))
		SDL_QuitSubSystem(SDL_INIT_VIDEO);

	if(!SDL_WasInit(SDL_INIT_VIDEO))
		SDL_Quit();
}

VkSurfaceKHR RenderWindow::createSurface(VkInstance instance)
{
	VkSurfaceKHR result;
	if(logger.checkSDLError(SDL_Vulkan_CreateSurface(window, instance, nullptr, &result)))
		return VkSurfaceKHR{};

	return result;
}

std::pair<uint32_t, uint32_t> RenderWindow::getWindowSize() const
{
	int width{}, height{};
	SDL_GetWindowSizeInPixels(window, &width, &height);
	return {width, height};
}