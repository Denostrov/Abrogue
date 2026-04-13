module Abrogue:Constants;

import Helpers;

using namespace std::literals;

//Class for storing used constant values
class Constants
{
public:
    static constexpr auto configFileName{"config.json"sv};
    static constexpr auto dataFileName{"data.json"sv};

    static constexpr auto appName{"Abrogue"sv};
    static constexpr auto appVersion{"0.1"sv};
    static constexpr auto appIdentifier{"org.strovstiksoft.abrogue"sv};
    static constexpr auto appCreator{"Strovstik Software Inc."sv};
    static constexpr auto appCopyright{"Copyright (c) 2025 Strovstik Software Inc."sv};
    static constexpr auto appURL{"https://github.com/Denostrov"sv};
    static constexpr auto appType{"game"sv};

    static constexpr std::uint32_t vkAppMajorVersion{0u};
    static constexpr std::uint32_t vkAppMinorVersion{1u};
    static constexpr std::uint32_t vkAppPatchVersion{0u};

    static constexpr std::int64_t ticksPerSecond{8};
    static constexpr std::int64_t tickDurationNS{1000000000 / ticksPerSecond};
    static constexpr double tickDuration{1.0 / ticksPerSecond};

    static constexpr std::int64_t screenWidth{128};
    static constexpr std::int64_t screenHeight{36};

    static constexpr std::int64_t mapWidth{80};
    static constexpr std::int64_t mapHeight{36};
    static constexpr std::size_t mapTileCount{mapWidth * mapHeight};
    static constexpr std::int64_t mapOffset{screenWidth - mapWidth};
    static constexpr double mapMinBrightness{0.25};
    static constexpr double mapInLineOfSightValue{0.125};

    static constexpr float tileAspectRatio{0.5f};
    static constexpr float tileScaleX{tileAspectRatio / screenHeight};
    static constexpr float tileScaleY{1.0f / screenHeight};

    static constexpr PackedColor labelBackgroundColor{Color::pack(255, 255, 255, 0)};
    static constexpr PackedColor labelHoveredColor{Color::pack(8, 8, 8, 255)};
    static constexpr PackedColor labelPressedColor{Color::pack(16, 16, 16, 255)};
    static constexpr PackedColor labelHoveredPressedColor{Color::pack(24, 24, 24, 255)};
    static constexpr PackedColor healthBackgroundColor{Color::pack(128, 0, 0, 255)};
    static constexpr PackedColor healthHoverColor{Color::pack(160, 0, 0, 255)};
    static constexpr PackedColor nutritionBackgroundColor{Color::pack(16, 16, 128, 255)};
    static constexpr PackedColor nutritionHoverColor{Color::pack(32, 32, 160, 255)};
};

//Enum for visual types of weapons
enum class WeaponType
{
    eClaw,
    eClub,
    eDagger
};

//Enum for controls mapped to player input
enum class InputControlType
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
    COUNT
};

//Struct for defining an enemy type
struct EnemyData
{
    FixedString<16> name;
    std::uint8_t symbol{};
    Color color{};
    double speed{10.0};
    double mass{10.0};
    WeaponType weaponType{};
    Color weaponColor{};
    std::int64_t damage{};
    double attackTime{0.25};
};