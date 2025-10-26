module Abrogue:MainMenuImpl;

import :MainMenu;
import :GUI;
import :Game;

using namespace std::literals;

void MainMenu::init()
{
    using enum MainMenuButtonType;

    buttons[eStartGame].init("New game"sv, 120, 30);
    buttons[eOptions].init("Options"sv, 121, 31);
    buttons[eQuitToDesktop].init("Quit to desktop"sv, 113, 32);
}

void MainMenu::onButtonPressed(MainMenuButtonType type)
{
    using enum MainMenuButtonType;

    if(type == eStartGame)
        game.startGame();
    else if(type == eOptions)
        gui.showOptionsMenu();
    else if(type == eQuitToDesktop)
        game.quitToDesktop();
}