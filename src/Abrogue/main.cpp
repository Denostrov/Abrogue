#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>

#include <SDL3/SDL_messagebox.h>

import Game;
import InputHandler;

SDL_AppResult SDL_AppInit(void** appstate, int argc, char** argv)
{
	if(!game.init())
		return SDL_APP_FAILURE;

	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate)
{
	if(!game.update())
		return SDL_APP_FAILURE;

	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
	switch(event->type)
	{
		case SDL_EVENT_QUIT:
			return SDL_APP_SUCCESS;
		case SDL_EVENT_KEY_DOWN:
			if((event->key.mod & SDL_KMOD_SHIFT) == 0)
				inputHandler.onButtonPressed(event->key.scancode, true);
			else
				inputHandler.onShiftButtonPressed(event->key.scancode);
			break;
		case SDL_EVENT_KEY_UP:
			inputHandler.onButtonPressed(event->key.scancode, false);
			break;
		case SDL_EVENT_MOUSE_BUTTON_DOWN:
			if(event->button.button == 1)
				inputHandler.onMousePressed(event->button.x, event->button.y);
			break;
		case SDL_EVENT_MOUSE_MOTION:
			inputHandler.onMouseMoved(event->motion.x, event->motion.y);
			break;
		default: break;
	}

	if(game.getShouldExit())
		return SDL_APP_SUCCESS;

	return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
	
}