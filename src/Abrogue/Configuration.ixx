module Abrogue:Configuration;

import :Constants;

import JSON;

using namespace std::literals;

/*
 * Configuration - class for storing and updating configuration options
 */
class Configuration
{
public:
    //Check if loading and saving configuration files works
    [[nodiscard]] bool init();

    //Save current configuration to file
    bool saveOptions();

    [[nodiscard]] auto getWindowWidth() const { return windowWidth; }
    [[nodiscard]] auto getWindowHeight() const { return windowHeight; }
    [[nodiscard]] auto getIsFullscreen() const { return isFullscreen; }
    [[nodiscard]] auto getIsMaximized() const { return isMaximized; }
    [[nodiscard]] auto getInputControlFromScancode(SDL_Scancode scancode) const { return scancodeToInputControl[scancode]; }
    [[nodiscard]] auto getScancodeFromInputControl(InputControlType type) const { return inputControlToScancode[type]; }

    //Update values for current window size and state
    void updateWindowOptions();

    //Get printable string of scancode corresponding to control type
    [[nodiscard]] std::string_view getInputControlName(InputControlType type) const;

    //Modify control options
    void setInputControlScancode(InputControlType type, SDL_Scancode scancode);
    //Restore control options
    void resetInputControlsToDefault();

    //Get a random enemy to spawn at the current map floor
    [[nodiscard]] optCRef<EnemyData> getSuitableEnemy();

private:
    static nlohmann::json openJSONFile(std::string_view fileName);
    template <class Value>
    void readJSONValue(nlohmann::json const& json, std::string_view key, Value& value);

    bool loadOptions();
    void loadData();

    void setDefaultControls();

    std::int64_t windowWidth{800};
    std::int64_t windowHeight{450};
    bool isFullscreen{};
    bool isMaximized{};

    Array<InputControlType, SDL_SCANCODE_COUNT> scancodeToInputControl{InputControlType::COUNT, FillTag{}};
    Array<SDL_Scancode, InputControlType::COUNT> inputControlToScancode{SDL_SCANCODE_UNKNOWN, FillTag{}};

    FixedVector<EnemyData, 128uz> enemyData;

    static constexpr Array<SDL_Scancode, InputControlType::COUNT> defaultControls{
        std::pair{InputControlType::eMoveUp, SDL_SCANCODE_W},
        std::pair{InputControlType::eMoveDown, SDL_SCANCODE_S},
        std::pair{InputControlType::eMoveLeft, SDL_SCANCODE_A},
        std::pair{InputControlType::eMoveRight, SDL_SCANCODE_D},
        std::pair{InputControlType::eAttack, static_cast<SDL_Scancode>(301)},
        std::pair{InputControlType::ePause, SDL_SCANCODE_SPACE},
        std::pair{InputControlType::eSearch, SDL_SCANCODE_Z},
        std::pair{InputControlType::eDiscoveries, SDL_SCANCODE_C},
        std::pair{InputControlType::eDebug, SDL_SCANCODE_F3},
        std::pair{InputControlType::eStopTime, SDL_SCANCODE_KP_7},
        std::pair{InputControlType::eStepTime, SDL_SCANCODE_KP_8}
    };
};
inline Configuration configuration;
