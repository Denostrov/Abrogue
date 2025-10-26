export module Abrogue;

export import :Game;
export import :InputHandler;

import :RenderWindow;

/*
 * GameSystems - class for handling global variable destruction order
 */
export class GameSystems
{
public:
    static void cleanup()
    {
        globalHandler.cleanup();
    }

private:
    inline static GlobalHandler<Configuration, QuadPool, RenderWindow, RenderEngine, InputHandler, Game, GUI> globalHandler{
        configuration, quadPool, renderWindow, renderEngine, inputHandler, game, gui
    };
};