module Abrogue:MainMenu;

import :ScreenComponent;

enum class MainMenuButtonType
{
    eStartGame,
    eOptions,
    eQuitToDesktop,
    COUNT
};

//Class for main menu screen
class MainMenu: public ScreenComponent<MainMenu, EmptyEnumType, MainMenuButtonType>
{
public:
    void init();

    void onButtonPressed(ButtonType type);
};