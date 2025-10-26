module Abrogue:PauseMenu;

import :ScreenComponent;

enum class PauseMenuButtonType
{
    eResume,
    eOptions,
    eAbandonGame,
    eSaveAndQuit,
    COUNT
};

//Class for pause menu
class PauseMenu: public ScreenComponent<PauseMenu, EmptyEnumType, PauseMenuButtonType>
{
public:
    void init();

    void onButtonPressed(ButtonType type) const;
};