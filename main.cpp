#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>

#include <SDL3/SDL_messagebox.h>

import RenderEngine;

SDL_AppResult SDL_AppInit(void** appstate, int argc, char** argv)
{
	auto checkErrorOccured = [](bool checkValue)
	{
		if(checkValue)
			return false;

		std::string errorString = SDL_GetError();
		SDL_ClearError();

		if(errorString.empty())
			return true;

		if(SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "SDL Error", errorString.c_str(), nullptr))
			return true;

		std::println(std::cerr, "{}", errorString);
		return true;
	};

	if(checkErrorOccured(SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_NAME_STRING, "Abrogue 0.1")))
		return SDL_APP_FAILURE;

	if(checkErrorOccured(SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_VERSION_STRING, "0.1")))
		return SDL_APP_FAILURE;

	if(checkErrorOccured(SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_IDENTIFIER_STRING, "org.strovstiksoft.abrogue")))
		return SDL_APP_FAILURE;

	if(checkErrorOccured(SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_CREATOR_STRING, "Strovstik Software Inc.")))
		return SDL_APP_FAILURE;

	if(checkErrorOccured(SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_COPYRIGHT_STRING, "Copyright (c) 2025 Strovstik Software Inc.")))
		return SDL_APP_FAILURE;

	if(checkErrorOccured(SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_URL_STRING, "https://github.com/Denostrov")))
		return SDL_APP_FAILURE;

	if(checkErrorOccured(SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_TYPE_STRING, "game")))
		return SDL_APP_FAILURE;

	if(checkErrorOccured(SDL_Init(SDL_INIT_VIDEO)))
		return SDL_APP_FAILURE;

	auto window = SDL_CreateWindow("Abrogue 0.1", 1600, 900, SDL_WINDOW_VULKAN);
	if(checkErrorOccured(window))
		return SDL_APP_FAILURE;

	renderEngine.init(window);

	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate)
{
	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
	return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
	SDL_DestroyWindow(renderEngine.getWindow());

	renderEngine.release();
}