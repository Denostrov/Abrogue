module Abrogue:OptionsMenu;

import :ScreenComponent;

enum class OptionsMenuLabelType
{
    eControls,
    eVideo,
    eResolution,
    COUNT
};

enum class OptionsMenuButtonType
{
    eMoveUp,
    eMoveDown,
    eMoveLeft,
    eMoveRight,
    eAttack,
    ePause,
    eSearch,
    eDiscoveries,
    eDebug,
    eStopTime,
    eStepTime,
    eResetToDefault,
    eFullscreen,
    COUNT
};

class OptionsMenu: public ScreenComponent<OptionsMenu, OptionsMenuLabelType, OptionsMenuButtonType>
{
public:
    void init();

    void onButtonPressed(ButtonType type);

    void refreshLabels();
};