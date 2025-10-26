module Abrogue:PauseMenuImpl;

import :PauseMenu;
import :GUI;
import :Game;

using namespace std::literals;

void PauseMenu::init()
{
    using enum ButtonType;

    buttons[eResume].init("Resume"sv, 60, 15);
    buttons[eOptions].init("Options"sv, 60, 16);
    buttons[eAbandonGame].init("Abandon game"sv, 57, 17);
    buttons[eSaveAndQuit].init("Save and quit"sv, 57, 18);
}

void PauseMenu::onButtonPressed(ButtonType type) const
{
    using enum ButtonType;

    if(type == eResume)
        gui.showPlayArea();
    else if(type == eOptions)
        gui.showOptionsMenu();
    else if(type == eAbandonGame || type == eSaveAndQuit)
        game.quitToMainMenu();
}