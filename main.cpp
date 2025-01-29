#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>

SDL_AppResult SDL_AppInit(void** appstate, int argc, char** argv)
{
	if(!SDL_SetAppMetadata("Abrogue", "0.1", "org.strovstiksoft.abrogue")) 
		return SDL_APP_FAILURE;

	if(!SDL_Init(SDL_INIT_VIDEO)) 
		return SDL_APP_FAILURE;

	auto window = SDL_CreateWindow("Abrogue 0.1", 1600, 900, SDL_WINDOW_VULKAN);
	if (!window)
		return SDL_APP_FAILURE;

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

}