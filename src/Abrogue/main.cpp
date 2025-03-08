#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>

#include <SDL3/SDL_messagebox.h>

import Logger;
import Configuration;
import Game;
import ObjectPools;

SDL_AppResult SDL_AppInit(void** appstate, int argc, char** argv)
{
	if(!Game::init())
		return SDL_APP_FAILURE;

	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate)
{
	if(!Game::update())
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
			Game::onKeyPressed(event->key.scancode);
			break;
		case SDL_EVENT_KEY_UP:
			Game::onKeyReleased(event->key.scancode);
			break;
		case SDL_EVENT_MOUSE_BUTTON_DOWN:
			if(event->button.button == 1)
				Game::onMousePressed(event->button.x, event->button.y);
			break;
		default: break;
	}

	return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
	Game::release();
}