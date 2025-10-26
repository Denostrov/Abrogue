module Abrogue:DebugMenu;

import :ScreenComponent;

//Enum for debug menu buttons
enum class DebugMenuButtonType
{
    eTitle,
    eStopTime,
    eStepTime,
    eShowDamage,
    eShowViewcone,
    eShowEnemies,
    COUNT
};

/*
 * Class for a popup menu with debugging options
 */
class DebugMenu: public ScreenComponent<DebugMenu, EmptyEnumType, DebugMenuButtonType>
{
public:
    //Initialize the buttons
    void init();

    //Enable or disable the corresponding debug option
    void onButtonPressed(ButtonType type);

    //Disable all debug options
    void resetToDefault();

    //Update button hotkey labels
    void refreshLabels();
};
