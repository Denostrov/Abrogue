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

//Class for a popup menu with debugging options
class DebugMenu: public ScreenComponent<DebugMenu, EmptyEnumType, DebugMenuButtonType>
{
public:
    void init();

    void onButtonPressed(ButtonType type);

    void resetToDefault();

    void refreshLabels();
};
