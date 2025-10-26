import Abrogue;
import SDL3;

SDL_AppResult SDL_AppInit(void**, int, char**)
{
    if (!game.init())
        return SDL_APP_FAILURE;

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void*)
{
    if (!game.update())
        return SDL_APP_FAILURE;

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void*, SDL_Event* event)
{
    switch (event->type)
    {
    case SDL_EVENT_QUIT:
        return SDL_APP_SUCCESS;
    case SDL_EVENT_WINDOW_RESIZED:
        game.refreshWindowState();
        break;
    case SDL_EVENT_KEY_DOWN:
        inputHandler.onButtonPressed(event->key.scancode, true);
        break;
    case SDL_EVENT_KEY_UP:
        inputHandler.onButtonPressed(event->key.scancode, false);
        break;
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
        inputHandler.onMousePressed(event->button.button, event->button.x, event->button.y);
        break;
    case SDL_EVENT_MOUSE_MOTION:
        inputHandler.onMouseMoved(event->motion.x, event->motion.y);
        break;
    default: break;
    }

    if (game.getShouldExit())
        return SDL_APP_SUCCESS;

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void*, SDL_AppResult)
{
    GameSystems::cleanup();
}
