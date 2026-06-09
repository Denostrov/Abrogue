export module Abrogue;

import RenderEngine;
import Helpers;
import JSON;

import std;

using namespace std::literals;

/*
 * Concepts for GUI
 */
template <class T>
concept HasButtons = (std::size_t)T::COUNT > 0;


/*
 * Common enums
 */

/*
 * Enum for visual types of weapons
 */
enum class WeaponType
{
    eClaw,
    eClub,
    eDagger
};
/*
 * Enum for controls mapped to player input
 */
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
/*
 * Enum for empty GUI
 */
enum class EmptyEnumType
{
    COUNT
};


/*
 * Common structs
 */

/*
 * Struct for defining an enemy type
 */
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


/*
 * Classes for game systems
 */

/*
 * Class for storing used constant values
 */
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

    static constexpr std::int64_t ticksPerSecond{16};
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
/*
 * Class for drawing lines when debugging
 */
class DebugLine
{
public:
    DebugLine() = default;
    DebugLine(double startX, double startY, double endX, double endY)
    {
        double distanceX = endX - startX;
        double distanceY = (endY - startY) * 2.0;
        double distance = std::sqrt(distanceX * distanceX + distanceY * distanceY);
        double cos = distanceX / distance;
        double sin = distanceY / distance;

        QuadData quadData{{startX + distanceX / 2.0, startY + distanceY / 4.0}, {Color::pack(255, 0, 0, 255), Color::pack(255, 0, 0, 255)}, 179};
        quadData.setRotation(static_cast<float>(-sin), static_cast<float>(cos));
        quadData.setScale(0.1f, static_cast<float>(distance / 2.0));
        quad.init(quadData);
    }

private:
    QuadReference<QuadLayer::eEntity> quad;
};
/*
 * Class for handling attacks and dealing damage
 */
class Weapon
{
public:
    Weapon() = default;
    void init(WeaponType newType, Color newColor, std::int64_t newDamage, double newAttackTime, bool friendly);

    [[nodiscard]] bool getIsAttacking() const { return attackTimer > 0.0; }

    void update(double positionX, double positionY);
    void updateDraw(double positionX, double positionY);

    void startAttack(double positionX, double positionY, double targetPositionX, double targetPositionY);

    static void setDrawDebug(bool draw) { drawDebugInfo = draw; }

private:
    inline static bool drawDebugInfo{};

    WeaponType type;
    Color color;
    std::int64_t damage{};
    double attackTime{};
    double drawOffset{};
    bool isFriendly{};

    QuadReference<QuadLayer::eItem> weaponReference;
    double attackTimer{};
    double attackAngleCos{};
    double attackAngleSin{};

    QuadReference<QuadLayer::eMapOverlay> damageReference;
};
/*
 * Class for handling animating objects
 */
class AnimationHandler
{
public:
    AnimationHandler(std::uint64_t endTimeNS) : endTimeNS(endTimeNS), endTime(endTimeNS / 1.e9) {}

    std::uint64_t update()
    {
        logger.extraAssert(!isFinished, "AnimationHandler updated after finishing");

        currentTimeNS += Constants::tickDurationNS * timeDirection;
        currentTime = currentTimeNS / 1.e9;
        if (currentTimeNS > endTimeNS)
        {
            currentTimeNS = endTimeNS;
            isFinished = true;
        }
        else if (currentTimeNS < 0)
        {
            currentTimeNS = 0;
            isFinished = true;
        }

        return currentTimeNS * 1000 / endTimeNS;
    }

    double updateDraw(double deltaTime)
    {
        logger.extraAssert(!isFinished, "AnimationHandler updated draw after finishing");

        double extrapolatedTime = (currentTime + deltaTime * timeDirection) / endTime;
        return std::clamp(extrapolatedTime, 0.0, 1.0);
    }

    void startAnimation(bool forward)
    {
        std::int64_t newTimeDirection = forward ? 1 : -1;
        logger.extraAssert(newTimeDirection != timeDirection, "AnimationHandler started animation in same direction");

        timeDirection = newTimeDirection;
        isFinished = false;
    }

    [[nodiscard]] auto getIsFinished() const { return isFinished; }
    [[nodiscard]] auto getIsForward() const { return timeDirection == 1; }

private:
    std::int64_t endTimeNS{};
    double endTime{};

    std::int64_t currentTimeNS{};
    double currentTime{};

    std::int64_t timeDirection{-1};
    bool isFinished{true};
};
/*
 * Class for handling movement and collisions
 */
class PhysicsComponent
{
public:
    PhysicsComponent() = default;
    PhysicsComponent(double x, double y, double leftScaleX, double rightScaleX, double topScaleY, double bottomScaleY);

    [[nodiscard]] std::pair<double, double> getPosition() const { return {x, y}; }
    [[nodiscard]] std::pair<double, double> getVelocity() const { return {velocityX, velocityY}; }
    [[nodiscard]] std::pair<double, double> getPredictedPosition(double deltaTime) const
    {
        auto interpolationCoefficient = deltaTime / Constants::tickDuration;
        return {x + (nextX - x) * interpolationCoefficient, y + (nextY - y) * interpolationCoefficient};
    }

protected:
    void setMass(double newMass) { mass = newMass; }
    void setFrictionCoefficient(double newFriction) { frictionCoefficient = newFriction; }
    void setMaxVelocity(double newMaxSpeed);
    void setMovementDirection(double directionX, double directionY);

    void update();

private:
    void calculateNextStep();

    double x{}, y{};                                          // Coordinates in [0.0, mapWidth]x[0.0, mapHeight] space
    double velocityX{}, velocityY{};                          // Velocity in horizontal tiles per second
    double mass{1.0};                                         // Measures resistance to acceleration
    double frictionCoefficient{1.0};                          // Controls effectiveness of acceleration and deceleration
    double maxVelocity{1.0};                                  // Speed when in equilibrium with friction
    double resistanceCoefficient{20.0};                       // Controls effectiveness of deceleration due to velocity
    double walkingForce{maxVelocity * resistanceCoefficient}; // Force necessary to achieve max velocity

    double nextX{}, nextY{};
    double nextVelocityX{}, nextVelocityY{};

    double leftScaleX{0.45}, rightScaleX{0.45}; // Horizontal hitbox extents
    double topScaleY{0.45}, bottomScaleY{0.45}; // Vertical hitbox extents

    double movementDirectionX{}, movementDirectionY{}; // Unscaled directions of applied movement forces
};
/*
 * Item - class for handling items
 */
class Item : public PhysicsComponent
{
public:
    enum class Type
    {
        eGold,
        eFood,
        eAmulet,
        COUNT
    };

    Item() = default;
    Item(Type newType, double x, double y);

    void update();
    void updateDraw(double deltaTime);

    [[nodiscard]] Type getType() const { return type; }
    [[nodiscard]] FixedString<32> getName() const;

    void setVisible(bool visible);

private:
    static constexpr std::array<std::uint32_t, (std::size_t)Type::COUNT> typeGlyphs{42, 59, 157};

    Type type{};

    QuadReference<QuadLayer::eItem> quad;
};
/*
 * Class for handling user input
 */
export class InputHandler
{
public:
    InputHandler() = default;

    void setChangingControlType(InputControlType type) { changingControlType = type; }

    [[nodiscard]] std::pair<float, float> getMousePosition() const;
    void onMouseMoved(float x, float y);
    void onMousePressed(std::uint8_t buttonIndex, float x, float y);

    void onButtonPressed(SDL_Scancode scancode, bool pressed);

private:
    std::array<bool, SDL_SCANCODE_COUNT> pressedButtons{};
    InputControlType changingControlType{InputControlType::COUNT};
};
export inline InputHandler inputHandler;
/*
 * Class for storing and updating configuration options
 */
class Configuration
{
public:
    // Check if loading and saving configuration files works
    [[nodiscard]] bool init();

    // Save current configuration to file
    bool saveOptions();

    [[nodiscard]] auto getWindowWidth() const { return windowWidth; }
    [[nodiscard]] auto getWindowHeight() const { return windowHeight; }
    [[nodiscard]] auto getIsFullscreen() const { return isFullscreen; }
    [[nodiscard]] auto getIsMaximized() const { return isMaximized; }
    [[nodiscard]] auto getInputControlFromScancode(SDL_Scancode scancode) const { return scancodeToInputControl[scancode]; }
    [[nodiscard]] auto getScancodeFromInputControl(InputControlType type) const { return inputControlToScancode[type]; }

    // Update values for current window size and state
    void updateWindowOptions();

    // Get printable string of scancode corresponding to control type
    [[nodiscard]] std::string_view getInputControlName(InputControlType type) const;

    // Modify control options
    void setInputControlScancode(InputControlType type, SDL_Scancode scancode);
    // Restore control options
    void resetInputControlsToDefault();

    // Get a random enemy to spawn at the current map floor
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
        SDL_SCANCODE_W, SDL_SCANCODE_S,  SDL_SCANCODE_A,    SDL_SCANCODE_D,   static_cast<SDL_Scancode>(301), SDL_SCANCODE_SPACE, SDL_SCANCODE_Z,
        SDL_SCANCODE_C, SDL_SCANCODE_F3, SDL_SCANCODE_KP_7, SDL_SCANCODE_KP_8};
};
inline Configuration configuration;
/*
 * Map - class for handling gameplay area
 */
class Map
{
    class Tile
    {
    public:
        enum class Type
        {
            eFloor,
            eGrass,
            eBush,
            eWall,
            eBedrock,
            eDoor,
            eExit,
            COUNT
        };

        Tile() = default;
        Tile(std::int64_t x, std::int64_t y, Type type);

        void updateDraw(double brightness)
        {
            auto color = info[(std::size_t)type].color;
            auto backgroundColor = info[(std::size_t)type].backgroundColor;

            color.multiplyRGB(brightness);
            backgroundColor.multiplyRGB(brightness);

            quad.setColor(color.getPacked());
            quad.setBackgroundColor(backgroundColor.getPacked());
        }

        bool getIsSolid() const { return type == Type::eBedrock || type == Type::eWall || type == Type::eExit; }

        bool getIsOpaque() const { return type == Type::eBedrock || type == Type::eWall || type == Type::eBush || type == Type::eDoor || type == Type::eExit; }

        bool getIsFloor() const { return type == Type::eFloor || type == Type::eGrass || type == Type::eBush; }

        void setType(Type newType)
        {
            type = newType;
            quad.setGlyph(info[(std::size_t)type].glyph);
        }

    private:
        struct Info
        {
            Color color{};
            Color backgroundColor{};
            std::uint32_t glyph{};
        };

        static constexpr std::array<Info, (std::size_t)Type::COUNT> info{
            Info{Color(96, 96, 96, 255), Color(8, 8, 8, 255), 250},       Info{Color(0, 255, 0, 255), Color(8, 8, 8, 255), 34},
            Info{Color(0, 255, 0, 255), Color(8, 8, 8, 255), 237},        Info{Color(255, 255, 255, 255), Color(32, 32, 32, 255), 35},
            Info{Color(255, 255, 255, 255), Color(16, 16, 16, 255), 35},  Info{Color(255, 128, 0, 255), Color(192, 16, 0, 255), 43},
            Info{Color(255, 255, 255, 255), Color(16, 16, 192, 255), 234}};

        QuadReference<QuadLayer::eMap> quad;
        Type type{Type::COUNT};
    };

    struct Room
    {
        std::int64_t originX{}, originY{};
        std::int64_t width{}, height{};
    };

    struct LevelData
    {
        std::array<Room, 64> rooms{};
        std::uint64_t roomCount{};

        std::array<bool, Constants::mapTileCount> tilesOccupiedMask{};
    };

    struct DistanceFieldTile
    {
        std::int64_t kingDistance{};
    };

public:
    Map() = default;
    void init();

    void update();
    void updateDraw(double deltaTime);

    [[nodiscard]] std::optional<Item> pickupItem(std::int64_t x, std::int64_t y, bool onlyGold);

    [[nodiscard]] std::int64_t getDepth() const { return 1; }
    [[nodiscard]] Room const& getRandomRoom() const;
    [[nodiscard]] FixedVector<std::pair<std::int64_t, std::int64_t>, 32> getPath(std::int64_t startX, std::int64_t startY, std::int64_t endX,
                                                                                 std::int64_t endY) const;

    [[nodiscard]] bool getTileSolid(std::int64_t x, std::int64_t y) const;
    [[nodiscard]] bool getTileOpaque(std::int64_t x, std::int64_t y) const;
    [[nodiscard]] bool getTileFloor(std::int64_t x, std::int64_t y) const;
    [[nodiscard]] double getTileBrightness(std::int64_t x, std::int64_t y) const;
    [[nodiscard]] bool getTileInLineOfSight(std::int64_t x, std::int64_t y) const;

    static void setDrawDebugViewcone(bool draw) { updateVisibilityFunc = draw ? &Map::updateVisibilityDebug : &Map::updateVisibility; }

private:
    decltype(auto) getTile(this auto& self, std::int64_t x, std::int64_t y) { return self.tiles[x + y * Constants::mapWidth]; }

    void generateLevel();

    void updateVisibleTile(std::int64_t x, std::int64_t y, double distanceX, double distanceY, double visionRange);
    void updateVisibility(double deltaTime);
    void updateVisibilityDebug(double deltaTime);
    inline static void (Map::*updateVisibilityFunc)(double){};

    void calculateVisibilitySector(std::int64_t currentCellY, std::int64_t directionY, double startSlope, double startEnterX, std::int64_t startCellX,
                                   double endSlope, double endEnterX, std::int64_t endCellX, double playerX, double playerY, double visionRange);
    void calculateVisibilitySectorDebug(std::int64_t currentCellY, std::int64_t directionY, double startSlope, double startEnterX, std::int64_t startCellX,
                                        double endSlope, double endEnterX, std::int64_t endCellX, double playerX, double playerY, double visionRange);

    FixedVector<DebugLine, 256> debugLines;

    std::array<Tile, Constants::mapTileCount> tiles;
    LevelData levelData;

    FixedVector<std::uint64_t, Constants::mapTileCount> lastVisibleTiles;

    std::array<double, Constants::mapTileCount> tileBrightnessMask{};

    FixedVector<Item, 256> items;
};
inline Map map;
/*
 * Player - class for handling the main character
 */
class Player : public PhysicsComponent
{
public:
    Player() = default;
    Player(double velocity);

    void onMousePressed(std::uint32_t x, std::uint32_t y);

    void update();
    void updateDraw(double deltaTime);

    void takeDamage(std::int64_t damage);

    [[nodiscard]] auto getStealthRange() const { return stealthRange; }

    void setMovement(std::int64_t movementX, std::int64_t movementY);
    void setHealth(std::int64_t newHealth);

    Weapon weapon;
    std::int64_t health{};
    std::int64_t gold{};
    std::int64_t stealthRange{7};

    std::int64_t lastTileX{}, lastTileY{};

    FixedVector<Item, 20> inventory;
    bool hasAmulet{};

    QuadReference<QuadLayer::eEntity> quadReference;
};
inline Player player;
/*
 * Class for handling spawning of enemies
 */
class EnemyHandler
{
    class Enemy : public PhysicsComponent
    {
    public:
        enum class State
        {
            eSleeping,
            eWandering,
            eHunting
        };

        Enemy() = default;
        Enemy(EnemyData const& data, double positionX, double positionY, State initialState, bool isDrawDebug);

        void update(double playerX, double playerY, double playerVelocityX, double playerVelocityY, std::int64_t stealthRange);
        void updateDraw(double deltaTime);

        void updateDrawDebug();

    private:
        void setState(State newState);
        void setPathTo(std::int64_t x, std::int64_t y);

        State state;
        Weapon weapon;

        Color color;

        double stealthTimer{};
        double lastCheckedStealthTime{};

        FixedVector<std::pair<std::int64_t, std::int64_t>, 32> path;
        std::int64_t currentPathIndex{};

        QuadReference<QuadLayer::eEntity> quad;

        QuadReference<QuadLayer::eEntity> stateQuad;
        FixedVector<QuadReference<QuadLayer::eEntity>, 128> pathQuads;
    };

public:
    EnemyHandler() = default;

    void update();
    void updateDraw(double deltaTime);

    void inflictDamage(double damageX, double damageY);
    void populateLevel();

    void setDrawDebug(bool draw);

private:
    bool spawnEnemy();

    double currentTime{};

    FixedVector<Enemy, 512> enemies;
    double lastEnemySpawnTime{};

    bool isDrawDebug{};
};
inline EnemyHandler enemyHandler;
/*
 * Game - class for handling game initialization and logic
 */
export class Game
{
public:
    enum class State
    {
        eNotStarted,
        eRunning,
        ePaused,
        eFinished
    };

    [[nodiscard]] bool init();
    void cleanup() { renderEngine.cleanup(); }

    [[nodiscard]] bool update();
    void advanceStep() const;

    void startGame();
    void quitToMainMenu();
    void quitToDesktop();

    void setPaused(bool paused);
    void setSpeedPercentage(std::uint64_t speed);
    void setPlayerMovement(std::int64_t movementX, std::int64_t movementY) const;

    [[nodiscard]] bool getShouldExit() const { return state == State::eFinished; }

    void refreshWindowState() const;

private:
    [[nodiscard]] bool updateDraw(double deltaTime) const;

    std::uint64_t guiDeltaTimeNS{};
    std::uint64_t gameDeltaTimeNS{};
    std::uint64_t currentTimeNS{};

    std::uint64_t speedPercentage{100};
    std::uint64_t adjustedTickDuration{Constants::tickDurationNS};

    std::uint64_t framesDrawn{};
    std::uint64_t maxFrameTimeNS{1};
    std::uint64_t lastFPSLogTimeNS{};

    State state{};
};
export inline Game game;


/*
 * Classes for GUI objects
 */

/*
 * Class for a semi transparent rectangle that covers the whole screen
 * The rectangle fades in over time after being shown
 */
class Background
{
public:
    void update()
    {
        if (animationHandler.getIsFinished())
            return;

        auto result = animationHandler.update();
        if (result == 0 && !animationHandler.getIsForward())
            quad.clear();
    }

    void updateDraw(double deltaTime)
    {
        if (animationHandler.getIsFinished())
            return;

        auto extrapolatedTime = animationHandler.updateDraw(deltaTime);
        auto currentColor = extrapolatedTime * 240.0;
        quad.setBackgroundColor(Color::pack(0, 0, 0, currentColor));
    }

    void setVisible(bool visible)
    {
        if (animationHandler.getIsForward() == visible)
            return;

        animationHandler.startAnimation(visible);
        if (!quad)
        {
            quad.init(QuadData{{Constants::screenWidth / 2.0f, Constants::screenHeight / 2.0f},
                               {Color::pack(0, 0, 0, 0), Color::pack(0, 0, 0, 0)},
                               ' ',
                               {Constants::screenWidth, Constants::screenHeight}});
        }
    }

private:
    AnimationHandler animationHandler{100'000'000};
    QuadReference<QuadLayer::ePopupBackground> quad;
};
/*
 * Class for handling clickable text boxes
 */
template <QuadLayer layer>
class Label
{
public:
    Label() = default;

    void init(std::string_view text, std::int64_t x, std::int64_t y, bool visible = false)
    {
        setPosition(x, y);
        setText(text);

        setVisible(visible);
    }

    void update()
    {
        if (animationHandler.getIsFinished())
            return;

        animationHandler.update();
    }
    void updateDraw(double deltaTime)
    {
        if (animationHandler.getIsFinished())
            return;

        auto extrapolatedTime = animationHandler.updateDraw(deltaTime);
        if (extrapolatedTime <= -1.0)
            return;

        if (extrapolatedTime <= 0.0 && !animationHandler.getIsForward())
        {
            quadReferences.clear();
            return;
        }

        for (std::size_t i = 0; i < quadReferences.getSize(); i++)
        {
            quadReferences[i].setPosition(x + i + 0.5f, y + 0.5f + Constants::mapHeight * (1.0 - extrapolatedTime));
        }
    }

    [[nodiscard]] bool checkCollision(std::int64_t checkX, std::int64_t checkY) const
    {
        return x <= checkX && checkX < x + size && y <= checkY && checkY < y + 1;
    }

    void togglePressed() { setPressed(!isPressed); }

    [[nodiscard]] auto getPressed() const { return isPressed; }

    void setVisible(bool visible)
    {
        if (animationHandler.getIsForward() == visible)
            return;

        animationHandler.startAnimation(visible);

        // Invisible label has size 0 to disable collision detection
        isVisible = visible;
        size = visible ? text.getSize() : 0;

        // Delete invisible quads to avoid overdraw
        if (!quadReferences.isEmpty())
            return;

        // Recreate quads
        for (std::size_t i = 0; i < size; i++)
        {
            quadReferences.emplaceBack();
            quadReferences.getBack().init(QuadData{{x + i + 0.5f, y + 0.5f}, {Color::pack(255, 255, 255, 255), getBackgroundColor(i)}, (std::uint32_t)text[i]});
        }
    }
    void setHovered(bool hovered)
    {
        if (isHovered == hovered)
            return;

        isHovered = hovered;

        for (std::size_t i = 0; i < size; i++)
            quadReferences[i].setBackgroundColor(getBackgroundColor(i));
    }
    void setPressed(bool pressed)
    {
        if (isPressed == pressed)
            return;

        isPressed = pressed;

        for (std::size_t i = 0; i < size; i++)
            quadReferences[i].setBackgroundColor(getBackgroundColor(i));
    }

    void setText(std::string_view newText)
    {
        text = newText;
        if (!isVisible)
            return;

        size = text.getSize();

        // Create remaining quads when new text is longer
        for (std::size_t i = quadReferences.getSize(); i < size; ++i)
        {
            quadReferences.emplaceBack();
            quadReferences.getBack().init(QuadData{{x + i + 0.5f, y + 0.5f}, {Color::pack(255, 255, 255, 255), getBackgroundColor(i)}, (std::uint32_t)text[i]});
        }

        // Set existing quad parameters
        for (std::size_t i = 0; i < size; i++)
        {
            quadReferences[i].setGlyph(text[i]);
            quadReferences[i].setBackgroundColor(getBackgroundColor(i));
        }

        // Erase extra quads when new text is shorter
        quadReferences.resize(size);
    }
    void setPosition(std::int64_t newX, std::int64_t newY)
    {
        x = newX;
        y = newY;
        for (std::size_t i = 0; i < size; i++)
            quadReferences[i].setPosition(x + i + 0.5f, y + 0.5f);
    }
    void setBackgroundColor(PackedColor color, PackedColor hoverColor)
    {
        backgroundColor = color;
        hoveredBackgroundColor = hoverColor;

        for (std::size_t i = 0; i < size; i++)
            quadReferences[i].setBackgroundColor(getBackgroundColor(i));
    }
    void setPressedBackgroundColor(PackedColor color, PackedColor hoverColor)
    {
        pressedBackgroundColor = color;
        hoveredPressedBackgroundColor = hoverColor;

        for (std::size_t i = 0; i < size; i++)
            quadReferences[i].setBackgroundColor(getBackgroundColor(i));
    }
    void setProgress(double percentage)
    {
        progress = percentage;

        for (std::size_t i = 0; i < size; i++)
            quadReferences[i].setBackgroundColor(getBackgroundColor(i));
    }

private:
    [[nodiscard]] PackedColor getBackgroundColor(std::int64_t index) const
    {
        Color color = isPressed ? (isHovered ? hoveredPressedBackgroundColor : pressedBackgroundColor) : (isHovered ? hoveredBackgroundColor : backgroundColor);

        // Adjust colors to form a gradient from left to right depending on progress
        double colorCoefficient = std::clamp(progress * size - index, 0.0, 1.0);
        color.multiplyRGB(colorCoefficient);
        return color.getPacked();
    }

    AnimationHandler animationHandler{2'000'000'000};

    bool isVisible{};
    bool isHovered{};
    bool isPressed{};
    double progress{1.0};

    std::int64_t x{}, y{};
    std::size_t size{};

    PackedColor backgroundColor{Constants::labelBackgroundColor};
    PackedColor hoveredBackgroundColor{Constants::labelHoveredColor};
    PackedColor pressedBackgroundColor{Constants::labelPressedColor};
    PackedColor hoveredPressedBackgroundColor{Constants::labelHoveredPressedColor};

    FixedString<128> text;
    FixedVector<QuadReference<layer>, 128> quadReferences;
};
/*
 * Base class for GUI screens
 */
template <class Derived, class DerivedLabelType = EmptyEnumType, class DerivedButtonType = EmptyEnumType, class DerivedTabButtonType = EmptyEnumType,
          QuadLayer layer = QuadLayer::ePopup>
class ScreenComponent
{
public:
    using LabelType = DerivedLabelType;
    using ButtonType = DerivedButtonType;
    using TabButtonType = DerivedTabButtonType;

    void update()
    {
        if constexpr (HasButtons<LabelType>)
        {
            for (auto& label : labels)
                label.update();
        }

        if constexpr (HasButtons<ButtonType>)
        {
            for (auto& button : buttons)
                button.update();
        }

        if constexpr (HasButtons<TabButtonType>)
        {
            for (auto& tabButton : tabButtons)
                tabButton.update();
        }
    }

    void updateDraw(double deltaTime)
    {
        if constexpr (HasButtons<LabelType>)
        {
            for (auto& label : labels)
                label.updateDraw(deltaTime);
        }

        if constexpr (HasButtons<ButtonType>)
        {
            for (auto& button : buttons)
                button.updateDraw(deltaTime);
        }

        if constexpr (HasButtons<TabButtonType>)
        {
            for (auto& tabButton : tabButtons)
                tabButton.updateDraw(deltaTime);
        }
    }

    void setVisible(bool visible)
    {
        if constexpr (HasButtons<LabelType>)
        {
            for (auto& label : labels)
                label.setVisible(visible);
        }

        if constexpr (HasButtons<ButtonType>)
        {
            for (auto& button : buttons)
                button.setVisible(visible);
        }

        if constexpr (HasButtons<TabButtonType>)
        {
            for (auto& tabButton : tabButtons)
                tabButton.setVisible(visible);
        }
    }

    void updateMouseMoved(std::int64_t x, std::int64_t y)
    {
        auto handleButtonCollisions = [this, x, y](auto& buttons)
        {
            for (std::size_t i = 0; i < buttons.getSize(); i++)
            {
                if (!buttons[i].checkCollision(x, y))
                    continue;

                auto button = &buttons[i];
                if (button == hoveredButton)
                    return true;

                if (hoveredButton)
                    hoveredButton->setHovered(false);

                button->setHovered(true);
                hoveredButton = button;
                return true;
            }

            return false;
        };

        if constexpr (HasButtons<ButtonType>)
        {
            if (handleButtonCollisions(buttons))
                return;
        }

        if constexpr (HasButtons<TabButtonType>)
        {
            if (handleButtonCollisions(tabButtons))
                return;
        }

        if (!hoveredButton)
            return;

        hoveredButton->setHovered(false);
        hoveredButton = nullptr;
    }
    void updateMousePressed(std::int64_t x, std::int64_t y)
    {
        if constexpr (HasButtons<ButtonType>)
        {
            for (std::size_t i = 0; i < buttons.getSize(); i++)
            {
                if (!buttons[i].checkCollision(x, y))
                    continue;

                static_cast<Derived*>(this)->onButtonPressed((ButtonType)i);
                return;
            }
        }

        if constexpr (HasButtons<TabButtonType>)
        {
            for (std::size_t i = 0; i < tabButtons.getSize(); i++)
            {
                if (!tabButtons[i].checkCollision(x, y))
                    continue;

                if (pressedTabButtonType != TabButtonType::COUNT)
                    tabButtons[pressedTabButtonType].setPressed(false);

                pressedTabButtonType = (TabButtonType)i;

                if (pressedTabButtonType != TabButtonType::COUNT)
                {
                    tabButtons[i].setPressed(true);
                    static_cast<Derived*>(this)->onTabButtonPressed(pressedTabButtonType);
                }

                return;
            }
        }
    }

    void pressButton(ButtonType type)
        requires HasButtons<ButtonType>
    {
        logger.extraAssert(type < ButtonType::COUNT, "invalid button type"sv);

        static_cast<Derived*>(this)->onButtonPressed(type);
    }

    void setTabButtonPressed(TabButtonType type)
        requires HasButtons<TabButtonType>
    {
        if (pressedTabButtonType == type)
            return;

        if (pressedTabButtonType != TabButtonType::COUNT)
            tabButtons[pressedTabButtonType].setPressed(false);

        pressedTabButtonType = type;

        if (type != TabButtonType::COUNT)
        {
            static_cast<Derived*>(this)->onTabButtonPressed(pressedTabButtonType);
            tabButtons[pressedTabButtonType].setPressed(true);
        }
    }

protected:
    Array<Label<layer>, LabelType::COUNT> labels;
    Array<Label<layer>, ButtonType::COUNT> buttons;
    Array<Label<layer>, TabButtonType::COUNT> tabButtons;

private:
    Label<layer>* hoveredButton{};
    TabButtonType pressedTabButtonType{TabButtonType::COUNT};
};


/*
 * Classes for GUI screens
 */

/*
 * Enum for discovery labels
 */
enum class DiscoveriesLabelType
{
    eTitle,
    COUNT
};
/*
 * Class for a popup menu with discovered items
 */
class Discoveries : public ScreenComponent<Discoveries, DiscoveriesLabelType>
{
public:
    void init() { labels[LabelType::eTitle].init("Discoveries"sv, 56, 6); }
};

/*
 * Enum for pause menu buttons
 */
enum class PauseMenuButtonType
{
    eResume,
    eOptions,
    eAbandonGame,
    eSaveAndQuit,
    COUNT
};
/*
 * Class for pause menu
 */
class PauseMenu : public ScreenComponent<PauseMenu, EmptyEnumType, PauseMenuButtonType>
{
public:
    void init();

    void onButtonPressed(ButtonType type) const;
};

/*
 * Enum for main menu buttons
 */
enum class MainMenuButtonType
{
    eStartGame,
    eOptions,
    eQuitToDesktop,
    COUNT
};
/*
 * Class for main menu screen
 */
class MainMenu : public ScreenComponent<MainMenu, EmptyEnumType, MainMenuButtonType>
{
public:
    void init();

    void onButtonPressed(ButtonType type);
};

enum class PlayAreaButtonType
{
    ePause,
    eHealth,
    eNutrition,
    eGold,
    eInventory,
    eInventorySlotFirst = eInventory + 1,
    eInventorySlotLast = eInventorySlotFirst + 19,
    eDepth,
    eSearch,
    COUNT
};
enum class PlayAreaTabButtonType
{
    eDebug,
    eDiscoveries,
    eMenu,
    COUNT
};
/*
 * Class for play area
 */
class PlayArea : public ScreenComponent<PlayArea, EmptyEnumType, PlayAreaButtonType, PlayAreaTabButtonType, QuadLayer::eMap>
{
public:
    void init();

    void onButtonPressed(ButtonType type);
    void onTabButtonPressed(TabButtonType type) const;

    void updateInventory(FixedVector<Item, 20> const& inventory, std::int64_t gold);

    [[nodiscard]] bool getPaused() const { return buttons[ButtonType::ePause].getPressed(); }
    void setPaused(bool paused);

    void setPlayerHealth(double percentage);

    void refreshLabels();
};

/*
 * Enum for game over labels
 */
enum class GameOverLabelType
{
    eGameOver,
    ePressSpace,
    COUNT
};
/*
 * Class for a popup screen with game over text
 */
class GameOver : public ScreenComponent<GameOver, GameOverLabelType>
{
public:
    void init();

    void setWinner(bool winner);

    void refreshLabels();
};

/*
 * Enum for debug menu buttons
 */
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
class DebugMenu : public ScreenComponent<DebugMenu, EmptyEnumType, DebugMenuButtonType>
{
public:
    // Initialize the buttons
    void init();

    // Enable or disable the corresponding debug option
    void onButtonPressed(ButtonType type);

    // Disable all debug options
    void resetToDefault();

    // Update button hotkey labels
    void refreshLabels();
};

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
/*
 * Class for an options menu
 */
class OptionsMenu : public ScreenComponent<OptionsMenu, OptionsMenuLabelType, OptionsMenuButtonType>
{
public:
    void init();

    void onButtonPressed(ButtonType type);

    void refreshLabels();
};

/*
 * GUI - class for handling transitions between screens
 */
class GUI
{
    // Enum for different screen types
    enum class ScreenType
    {
        eNone,
        eMainMenu,
        eOptionsMenu,
        ePlayArea,
        ePauseMenu,
        eDiscoveries,
        eDebugMenu,
        eGameOver
    };

public:
    GUI() {}
    void init();

    void showPlayArea();
    void showMainMenu();
    void showOptionsMenu();
    void showGameOver(bool winner);

    void update();
    void updateDraw(double deltaTime);

    void onMouseMoved(std::int64_t x, std::int64_t y);
    void onMousePressed(std::int64_t x, std::int64_t y);

    void onPauseMenuHotkeyPressed();
    void onDebugHotkeyPressed();
    void onDiscoveriesHotkeyPressed();
    void onPauseHotkeyPressed();
    void onStopTimeHotkeyPressed();
    void onStepTimeHotkeyPressed();

    void refreshScreens();

    void setFPS(std::int64_t fps, std::int64_t minFPS);
    void setPlayerHealth(double percentage);
    void setInventory(FixedVector<Item, 20> const& inventory, std::int64_t gold);

private:
    void setCurrentScreen(ScreenType screenType);
    void setScreenVisible(ScreenType screenType, bool visible);
    bool isScreenAPopup(ScreenType screenType) const;

    void executeOnScreen(ScreenType screenType, auto func)
    {
        switch (screenType)
        {
        case ScreenType::eMainMenu:
            func(mainMenu);
            break;
        case ScreenType::eOptionsMenu:
            func(optionsMenu);
            break;
        case ScreenType::ePlayArea:
            func(playArea);
            break;
        case ScreenType::ePauseMenu:
            func(pauseMenu);
            break;
        case ScreenType::eDiscoveries:
            func(discoveries);
            break;
        case ScreenType::eDebugMenu:
            func(debugMenu);
            break;
        case ScreenType::eGameOver:
            func(gameOver);
            break;
        default:
            break;
        }
    }

    ScreenType activeScreenType{ScreenType::eNone};     // Screen that's currently active and receiving input
    ScreenType backgroundScreenType{ScreenType::eNone}; // Screen that's behind the currently active screen
    bool previouslyPaused{};                            // Pause state before the popup screen was shown

    // Available screens
    MainMenu mainMenu;
    OptionsMenu optionsMenu;
    PlayArea playArea;
    PauseMenu pauseMenu;
    Discoveries discoveries;
    DebugMenu debugMenu;
    GameOver gameOver;

    Background popupBackground;      // Semi transparent black box behind popup screens
    Label<QuadLayer::eMap> fpsLabel; // Debug label for showing fps
};
inline GUI gui;


/*
 * Definitions of class methods
 */

/*
 * Weapon implementation
 */
void Weapon::init(WeaponType newType, Color newColor, std::int64_t newDamage, double newAttackTime, bool friendly)
{
    type = newType;
    color = newColor;
    damage = newDamage;
    attackTime = newAttackTime;
    isFriendly = friendly;

    if (type == WeaponType::eClaw || type == WeaponType::eClub)
        drawOffset = 0.2;
    else if (type == WeaponType::eDagger)
        drawOffset = 0.4;
}
void Weapon::update(double positionX, double positionY)
{
    if (attackTimer <= 0.0)
        return;

    bool readyToDamage = attackTimer > attackTime / 2.0;
    attackTimer -= Constants::tickDuration;
    if (readyToDamage && attackTimer <= attackTime / 2.0)
    {
        double weaponX = positionX + 1.5 * attackAngleCos;
        double weaponY = positionY + 1.5 * attackAngleSin / 2.0;

        if (drawDebugInfo)
        {
            auto quadData = QuadData{{Constants::mapOffset + weaponX, weaponY}, {Color::pack(255, 0, 0, 255), Color::pack(255, 0, 0, 0)}, 249};
            damageReference.init(quadData);
        }

        if (isFriendly)
        {
            enemyHandler.inflictDamage(weaponX, weaponY);
        }
        else
        {
            auto [playerX, playerY] = player.getPosition();
            if (weaponX > playerX - 0.5 && weaponX < playerX + 0.5 && weaponY > playerY - 0.5 && weaponY < playerY + 0.5)
                player.takeDamage(damage);
        }
    }
    else if (drawDebugInfo)
    {
        damageReference = {};
    }

    if (attackTimer <= 0.0)
    {
        attackTimer = 0.0;
        weaponReference = {};
    }
}
void Weapon::updateDraw(double positionX, double positionY)
{
    if (attackTimer <= 0.0)
        return;

    double attackPeak = attackTime / 2.0;
    double weaponOffset = (attackPeak - std::abs(attackTimer - attackPeak)) / attackPeak + drawOffset;
    weaponReference.setPosition(positionX + weaponOffset * attackAngleCos, positionY + weaponOffset * attackAngleSin * 0.5);
}
void Weapon::startAttack(double positionX, double positionY, double targetPositionX, double targetPositionY)
{
    attackTimer = attackTime;
    auto weaponGlyph = type == WeaponType::eDagger ? 24u : type == WeaponType::eClub ? 20u : 94u;
    QuadData weaponData{{positionX, positionY}, {color.getPacked(), color.getTransparentPacked()}, weaponGlyph};

    double distanceX = (targetPositionX - positionX);
    double distanceY = (targetPositionY - positionY) * 2.0;
    double distance = std::sqrt(distanceX * distanceX + distanceY * distanceY);
    attackAngleCos = distanceX / distance;
    attackAngleSin = distanceY / distance;
    weaponData.setRotation(-attackAngleSin, attackAngleCos);

    weaponReference.init(weaponData);
}
/*
 * PhysicsComponent implementation
 */
PhysicsComponent::PhysicsComponent(double x, double y, double leftScaleX, double rightScaleX, double topScaleY, double bottomScaleY) :
    x(x), y(y), leftScaleX(leftScaleX), rightScaleX(rightScaleX), topScaleY(topScaleY), bottomScaleY(bottomScaleY)
{
    calculateNextStep();
}
void PhysicsComponent::setMaxVelocity(double newMaxSpeed)
{
    maxVelocity = newMaxSpeed;
    walkingForce = maxVelocity * resistanceCoefficient;
}
void PhysicsComponent::setMovementDirection(double directionX, double directionY)
{
    movementDirectionX = directionX;
    movementDirectionY = directionY;

    calculateNextStep();
}
void PhysicsComponent::update()
{
    x = nextX;
    y = nextY;
    velocityX = nextVelocityX;
    velocityY = nextVelocityY;

    calculateNextStep();
}
void PhysicsComponent::calculateNextStep()
{
    auto integrate = [this](double coordinate, double velocity, double tileScale, double movementDirection)
    {
        std::pair result{coordinate, velocity};

        // Advance coordinate by half of time step
        result.first += velocity * Constants::tickDuration / 2.0;

        // Calculate forces
        double movementForce = movementDirection * walkingForce * frictionCoefficient;
        double frictionForce{};
        if (std::abs(velocity) > 0.0)
        {
            auto slowSpeed = std::max(0.2 * maxVelocity, std::abs(velocity));
            frictionForce = std::copysign(slowSpeed, -velocity) * frictionCoefficient * resistanceCoefficient;
        }

        // Remember velocity direction
        auto velocitySign = std::signbit(velocity);

        // Apply friction and movement force scaled by tile aspect ratio
        result.second += (movementForce * tileScale + frictionForce) / mass * Constants::tickDuration / 2.0;

        // Check if velocity inverted due to friction
        if (std::signbit(result.second) != velocitySign && std::abs(movementDirection) <= 0.001)
            result.second = 0.0;

        return result;
    };

    double directionAmplitude = std::sqrt(movementDirectionX * movementDirectionX + movementDirectionY * movementDirectionY);
    double movementX = directionAmplitude <= 0.001 ? 0 : movementDirectionX / directionAmplitude;
    double movementY = directionAmplitude <= 0.001 ? 0 : movementDirectionY / directionAmplitude;

    // Do forward euler integration in two steps to reflect coordinate change on same tick
    std::tie(nextX, nextVelocityX) = integrate(x, velocityX, 1.0, movementX);
    std::tie(nextY, nextVelocityY) = integrate(y, velocityY, Constants::tileAspectRatio, movementY);

    std::tie(nextX, nextVelocityX) = integrate(nextX, nextVelocityX, 1.0, movementX);
    std::tie(nextY, nextVelocityY) = integrate(nextY, nextVelocityY, Constants::tileAspectRatio, movementY);

    // No collisions possible if no movement
    if (nextX == x && nextY == y)
        return;

    // Calculate coefficients that convert orthogonal distances to distance along movement direction
    auto dx2 = (nextX - x) * (nextX - x);
    auto dy2 = (nextY - y) * (nextY - y);

    auto distanceCoefficientX = dx2 < 1.e-10 ? 100000.0 : std::sqrt(1 + dy2 / dx2);
    auto distanceCoefficientY = dy2 < 1.e-10 ? 100000.0 : std::sqrt(1 + dx2 / dy2);

    // Helper classes
    using TileCoords = std::pair<std::int32_t, std::int32_t>;
    struct Collision
    {
        enum Type
        {
            eNone,
            eHorizontal,
            eVertical
        } type{};                                            // Type of wall that was hit
        double positionX{}, positionY{};                     // Position of tile center during collision
        double distance{std::numeric_limits<double>::max()}; // Distance to collision point
    };

    // Current movement directions
    std::int32_t directionX = nextX >= x ? 1 : -1;
    std::int32_t directionY = nextY >= y ? 1 : -1;

    // Offsets for corners towards movement direction
    double positiveOffsetX = directionX == 1 ? rightScaleX : -leftScaleX;
    double negativeOffsetX = directionX == 1 ? -leftScaleX : rightScaleX;
    double positiveOffsetY = directionY == 1 ? bottomScaleY : -topScaleY;
    double negativeOffsetY = directionY == 1 ? -topScaleY : bottomScaleY;

    // Coordinates of corners towards movement direction
    double previousPositiveX = x + positiveOffsetX;
    double positiveX = nextX + positiveOffsetX;

    double previousNegativeX = x + negativeOffsetX;
    double negativeX = nextX + negativeOffsetX;

    double previousPositiveY = y + positiveOffsetY;
    double positiveY = nextY + positiveOffsetY;

    double previousNegativeY = y + negativeOffsetY;
    double negativeY = nextY + negativeOffsetY;

    auto calculateMinCollision = [this, directionX, directionY](double startX, double startY, double endX, double endY, double distanceCoefficientX,
                                                                double distanceCoefficientY, double offsetX, double offsetY, Collision& minCollision)
    {
        // Tiles corresponding to coordinates
        TileCoords startTile{startX, startY};
        TileCoords endTile{endX, endY};

        // Tiles haven't changed means no collision
        if (startTile == endTile)
            return;

        // Number of steps necessary to reach
        auto totalStepsX = std::abs(startTile.first - endTile.first);
        auto totalStepsY = std::abs(startTile.second - endTile.second);

        // Distances within starting tile
        double initialX{std::abs((directionX == 1) - (startX - startTile.first)) * distanceCoefficientX};
        double initialY{std::abs((directionY == 1) - (startY - startTile.second)) * distanceCoefficientY};

        // Step along movement direction until we reach the end tile
        std::uint32_t stepsX{}, stepsY{};
        while (totalStepsX > 0 || totalStepsY > 0)
        {
            // Pick the lowest distance along movement direction to find next tile
            double distanceX{initialX + distanceCoefficientX * stepsX}, distanceY{initialY + distanceCoefficientY * stepsY};
            if (totalStepsY == 0 || (distanceX <= distanceY && totalStepsX > 0))
            {
                startTile.first += directionX;
                stepsX++;
                totalStepsX--;
                // If exceeded already existing collision
                if (distanceX >= minCollision.distance)
                    return;

                // Record collision info if tile is solid
                if (map.getTileSolid(startTile.first, startTile.second))
                {
                    minCollision.type = Collision::eVertical;
                    minCollision.positionX = startTile.first + (directionX == -1) - offsetX * 1.001;
                    minCollision.positionY = startTile.second + 0.5;
                    minCollision.distance = distanceX;
                    return;
                }
            }
            else
            {
                startTile.second += directionY;
                stepsY++;
                totalStepsY--;
                if (distanceY >= minCollision.distance)
                    return;

                if (map.getTileSolid(startTile.first, startTile.second))
                {
                    minCollision.type = Collision::eHorizontal;
                    minCollision.positionX = startTile.first + 0.5;
                    minCollision.positionY = startTile.second + (directionY == -1) - offsetY * 1.001;
                    minCollision.distance = distanceY;
                    return;
                }
            }
        }
    };

    // Cast collision rays from 3 corners in the direction of movement
    Collision minCollision;
    calculateMinCollision(previousPositiveX, previousNegativeY, positiveX, negativeY, distanceCoefficientX, distanceCoefficientY, positiveOffsetX,
                          negativeOffsetY, minCollision);
    calculateMinCollision(previousNegativeX, previousPositiveY, negativeX, positiveY, distanceCoefficientX, distanceCoefficientY, negativeOffsetX,
                          positiveOffsetY, minCollision);
    calculateMinCollision(previousPositiveX, previousPositiveY, positiveX, positiveY, distanceCoefficientX, distanceCoefficientY, positiveOffsetX,
                          positiveOffsetY, minCollision);

    // If collision happened complete the movement step along the wall
    if (minCollision.type == Collision::eVertical)
    {
        // Reset collision info
        minCollision.type = Collision::eNone;
        minCollision.distance = std::numeric_limits<double>::max();

        // Set position along the wall
        nextVelocityX = 0.0;
        nextX = minCollision.positionX;

        // Cast ray from corner and check collision again
        calculateMinCollision(nextX + positiveOffsetX, minCollision.positionY + positiveOffsetY, nextX + positiveOffsetX, positiveY, 100000.0, 1.0,
                              positiveOffsetX, positiveOffsetY, minCollision);
        if (minCollision.type == Collision::eHorizontal)
        {
            nextVelocityY = 0.0;
            nextY = minCollision.positionY;
        }
    }
    else if (minCollision.type == Collision::eHorizontal)
    {
        minCollision.type = Collision::eNone;
        minCollision.distance = std::numeric_limits<double>::max();

        nextVelocityY = 0.0;
        nextY = minCollision.positionY;

        calculateMinCollision(minCollision.positionX + positiveOffsetX, y + positiveOffsetY, positiveX, y + positiveOffsetY, 1.0, 100000.0, positiveOffsetX,
                              positiveOffsetY, minCollision);
        if (minCollision.type == Collision::eVertical)
        {
            nextVelocityX = 0.0;
            nextX = minCollision.positionX;
        }
    }
}
/*
 * Item implementation
 */
Item::Item(Type newType, double x, double y) : PhysicsComponent(x, y, 0.48, 0.48, 0.48, 0.48)
{
    type = newType;

    quad.init(QuadData{{Constants::mapOffset + x, y}, {Color::pack(255, 255, 0, 255), Color::pack(255, 255, 0, 0)}, typeGlyphs[(std::size_t)newType]});
}
void Item::update() { PhysicsComponent::update(); }
void Item::updateDraw(double deltaTime)
{
    auto [x, y] = getPosition();
    auto [vx, vy] = getVelocity();
    quad.setPosition(Constants::mapOffset + x + vx * deltaTime, y + vy * deltaTime);

    auto brightness = map.getTileBrightness(x, y);
    if (brightness < Constants::mapMinBrightness)
    {
        quad.setColor(0);
        quad.setBackgroundColor(0);
    }
    else
    {
        Color itemColor(255, 255, 0, 255);
        itemColor.multiplyRGB(brightness);
        quad.setColor(itemColor.getPacked());
        quad.setBackgroundColor(itemColor.getTransparentPacked());
    }
}
FixedString<32> Item::getName() const
{
    FixedString<32> result;
    if (type == Type::eFood)
        result.fill("Food"sv);
    else if (type == Type::eAmulet)
        result.fill("Amulet of Yendor"sv);

    return result;
}
void Item::setVisible(bool visible)
{
    if (visible)
    {
        auto [x, y] = getPosition();
        quad.init(QuadData{{Constants::mapOffset + x, y}, {Color::pack(255, 255, 0, 255), Color::pack(255, 255, 0, 0)}, typeGlyphs[(std::size_t)type]});
    }
    else
        quad = QuadReference<QuadLayer::eItem>{};
}
/*
 * InputHandler implementation
 */
std::pair<float, float> InputHandler::getMousePosition() const
{
    float x{}, y{};
    SDL_GetMouseState(&x, &y);

    auto [width, height] = renderWindow.getWindowSize();
    return {x / width * Constants::screenWidth, y / height * Constants::screenHeight};
}
void InputHandler::onMouseMoved(float x, float y)
{
    auto [width, height] = renderWindow.getWindowSize();
    gui.onMouseMoved(x / width * Constants::screenWidth, y / height * Constants::screenHeight);
}
void InputHandler::onMousePressed(std::uint8_t buttonIndex, float x, float y)
{
    if (changingControlType != InputControlType::COUNT)
    {
        configuration.setInputControlScancode(changingControlType, (SDL_Scancode)(buttonIndex + 300));
        changingControlType = InputControlType::COUNT;
        return;
    }

    auto inputControl = configuration.getInputControlFromScancode((SDL_Scancode)(buttonIndex + 300));

    if (inputControl == InputControlType::eAttack)
    {
        auto [width, height] = renderWindow.getWindowSize();
        gui.onMousePressed(x / width * Constants::screenWidth, y / height * Constants::screenHeight);
    }
}
void InputHandler::onButtonPressed(SDL_Scancode scancode, bool pressed)
{
    pressedButtons[scancode] = pressed;

    if (pressed)
    {
        if (changingControlType != InputControlType::COUNT)
        {
            configuration.setInputControlScancode(changingControlType,
                                                  scancode == SDL_SCANCODE_ESCAPE ? configuration.getScancodeFromInputControl(changingControlType) : scancode);
            changingControlType = InputControlType::COUNT;
            return;
        }

        if (scancode == SDL_SCANCODE_ESCAPE)
        {
            gui.onPauseMenuHotkeyPressed();
            return;
        }

        auto inputControl = configuration.getInputControlFromScancode(scancode);

        if (inputControl == InputControlType::ePause)
            gui.onPauseHotkeyPressed();
        else if (inputControl == InputControlType::eDiscoveries)
            gui.onDiscoveriesHotkeyPressed();
        else if (inputControl == InputControlType::eDebug)
            gui.onDebugHotkeyPressed();
        else if (inputControl == InputControlType::eStopTime)
            gui.onStopTimeHotkeyPressed();
        else if (inputControl == InputControlType::eStepTime)
            gui.onStepTimeHotkeyPressed();
        else if (inputControl == InputControlType::eMoveUp || inputControl == InputControlType::eMoveLeft || inputControl == InputControlType::eMoveDown ||
                 inputControl == InputControlType::eMoveRight)
        {
            std::int64_t moveRight = pressedButtons[configuration.getScancodeFromInputControl(InputControlType::eMoveRight)];
            std::int64_t moveLeft = pressedButtons[configuration.getScancodeFromInputControl(InputControlType::eMoveLeft)];
            std::int64_t moveDown = pressedButtons[configuration.getScancodeFromInputControl(InputControlType::eMoveDown)];
            std::int64_t moveUp = pressedButtons[configuration.getScancodeFromInputControl(InputControlType::eMoveUp)];
            game.setPlayerMovement(moveRight - moveLeft, moveDown - moveUp);
        }
    }
    else
    {
        auto inputControl = configuration.getInputControlFromScancode(scancode);

        if (inputControl == InputControlType::eMoveUp || inputControl == InputControlType::eMoveLeft || inputControl == InputControlType::eMoveDown ||
            inputControl == InputControlType::eMoveRight)
        {
            std::int64_t moveRight = pressedButtons[configuration.getScancodeFromInputControl(InputControlType::eMoveRight)];
            std::int64_t moveLeft = pressedButtons[configuration.getScancodeFromInputControl(InputControlType::eMoveLeft)];
            std::int64_t moveDown = pressedButtons[configuration.getScancodeFromInputControl(InputControlType::eMoveDown)];
            std::int64_t moveUp = pressedButtons[configuration.getScancodeFromInputControl(InputControlType::eMoveUp)];
            game.setPlayerMovement(moveRight - moveLeft, moveDown - moveUp);
        }
    }
}
/*
 * Configuration implementation
 */
bool Configuration::init()
{
    if (!loadOptions())
        return false;

    loadData();

    return true;
}
void Configuration::updateWindowOptions()
{
    isFullscreen = renderWindow.getIsFullscreen();
    if (isFullscreen)
        return;

    isMaximized = renderWindow.getIsMaximized();
    if (isMaximized)
        return;

    std::tie(windowWidth, windowHeight) = renderWindow.getWindowSize();
}
std::string_view Configuration::getInputControlName(InputControlType type) const
{
    switch (inputControlToScancode[type])
    {
    case SDL_SCANCODE_A:
        return "[A]"sv;
    case SDL_SCANCODE_B:
        return "[B]"sv;
    case SDL_SCANCODE_C:
        return "[C]"sv;
    case SDL_SCANCODE_D:
        return "[D]"sv;
    case SDL_SCANCODE_E:
        return "[E]"sv;
    case SDL_SCANCODE_F:
        return "[F]"sv;
    case SDL_SCANCODE_G:
        return "[G]"sv;
    case SDL_SCANCODE_H:
        return "[H]"sv;
    case SDL_SCANCODE_I:
        return "[I]"sv;
    case SDL_SCANCODE_J:
        return "[J]"sv;
    case SDL_SCANCODE_K:
        return "[K]"sv;
    case SDL_SCANCODE_L:
        return "[L]"sv;
    case SDL_SCANCODE_M:
        return "[M]"sv;
    case SDL_SCANCODE_N:
        return "[N]"sv;
    case SDL_SCANCODE_O:
        return "[O]"sv;
    case SDL_SCANCODE_P:
        return "[P]"sv;
    case SDL_SCANCODE_Q:
        return "[Q]"sv;
    case SDL_SCANCODE_R:
        return "[R]"sv;
    case SDL_SCANCODE_S:
        return "[S]"sv;
    case SDL_SCANCODE_T:
        return "[T]"sv;
    case SDL_SCANCODE_U:
        return "[U]"sv;
    case SDL_SCANCODE_V:
        return "[V]"sv;
    case SDL_SCANCODE_W:
        return "[W]"sv;
    case SDL_SCANCODE_X:
        return "[X]"sv;
    case SDL_SCANCODE_Y:
        return "[Y]"sv;
    case SDL_SCANCODE_Z:
        return "[Z]"sv;
    case SDL_SCANCODE_1:
        return "[1]"sv;
    case SDL_SCANCODE_2:
        return "[2]"sv;
    case SDL_SCANCODE_3:
        return "[3]"sv;
    case SDL_SCANCODE_4:
        return "[4]"sv;
    case SDL_SCANCODE_5:
        return "[5]"sv;
    case SDL_SCANCODE_6:
        return "[6]"sv;
    case SDL_SCANCODE_7:
        return "[7]"sv;
    case SDL_SCANCODE_8:
        return "[8]"sv;
    case SDL_SCANCODE_9:
        return "[9]"sv;
    case SDL_SCANCODE_0:
        return "[0]"sv;
    case SDL_SCANCODE_RETURN:
        return "[RET]"sv;
    case SDL_SCANCODE_ESCAPE:
        return "[ESC]"sv;
    case SDL_SCANCODE_BACKSPACE:
        return "[BSPACE]"sv;
    case SDL_SCANCODE_TAB:
        return "[TAB]"sv;
    case SDL_SCANCODE_SPACE:
        return "[SPACE]"sv;
    case SDL_SCANCODE_MINUS:
        return "[-]"sv;
    case SDL_SCANCODE_EQUALS:
        return "[=]"sv;
    case SDL_SCANCODE_LEFTBRACKET:
        return "[{]"sv;
    case SDL_SCANCODE_RIGHTBRACKET:
        return "[}]"sv;
    case SDL_SCANCODE_BACKSLASH:
        return "[\\]"sv;
    case SDL_SCANCODE_SEMICOLON:
        return "[;]"sv;
    case SDL_SCANCODE_APOSTROPHE:
        return "[']"sv;
    case SDL_SCANCODE_GRAVE:
        return "[~]"sv;
    case SDL_SCANCODE_COMMA:
        return "[,]"sv;
    case SDL_SCANCODE_PERIOD:
        return "[.]"sv;
    case SDL_SCANCODE_SLASH:
        return "[/]"sv;
    case SDL_SCANCODE_CAPSLOCK:
        return "[CAPS]"sv;
    case SDL_SCANCODE_F1:
        return "[F1]"sv;
    case SDL_SCANCODE_F2:
        return "[F2]"sv;
    case SDL_SCANCODE_F3:
        return "[F3]"sv;
    case SDL_SCANCODE_F4:
        return "[F4]"sv;
    case SDL_SCANCODE_F5:
        return "[F5]"sv;
    case SDL_SCANCODE_F6:
        return "[F6]"sv;
    case SDL_SCANCODE_F7:
        return "[F7]"sv;
    case SDL_SCANCODE_F8:
        return "[F8]"sv;
    case SDL_SCANCODE_F9:
        return "[F9]"sv;
    case SDL_SCANCODE_F10:
        return "[F10]"sv;
    case SDL_SCANCODE_F11:
        return "[F11]"sv;
    case SDL_SCANCODE_F12:
        return "[F12]"sv;
    case SDL_SCANCODE_F13:
        return "[F13]"sv;
    case SDL_SCANCODE_F14:
        return "[F14]"sv;
    case SDL_SCANCODE_F15:
        return "[F15]"sv;
    case SDL_SCANCODE_F16:
        return "[F16]"sv;
    case SDL_SCANCODE_F17:
        return "[F17]"sv;
    case SDL_SCANCODE_F18:
        return "[F18]"sv;
    case SDL_SCANCODE_F19:
        return "[F19]"sv;
    case SDL_SCANCODE_F20:
        return "[F20]"sv;
    case SDL_SCANCODE_F21:
        return "[F21]"sv;
    case SDL_SCANCODE_F22:
        return "[F22]"sv;
    case SDL_SCANCODE_F23:
        return "[F23]"sv;
    case SDL_SCANCODE_F24:
        return "[F24]"sv;
    case SDL_SCANCODE_PRINTSCREEN:
        return "[PRINT]"sv;
    case SDL_SCANCODE_SCROLLLOCK:
        return "[SCROLL]"sv;
    case SDL_SCANCODE_PAUSE:
        return "[PAUSE]"sv;
    case SDL_SCANCODE_INSERT:
        return "[INSERT]"sv;
    case SDL_SCANCODE_HOME:
        return "[HOME]"sv;
    case SDL_SCANCODE_PAGEUP:
        return "[PGUP]"sv;
    case SDL_SCANCODE_DELETE:
        return "[DELETE]"sv;
    case SDL_SCANCODE_END:
        return "[END]"sv;
    case SDL_SCANCODE_PAGEDOWN:
        return "[PGDOWN]"sv;
    case SDL_SCANCODE_RIGHT:
        return "[RIGHT]"sv;
    case SDL_SCANCODE_LEFT:
        return "[LEFT]"sv;
    case SDL_SCANCODE_DOWN:
        return "[DOWN]"sv;
    case SDL_SCANCODE_UP:
        return "[UP]"sv;
    case SDL_SCANCODE_NUMLOCKCLEAR:
        return "[NLOCK]"sv;
    case SDL_SCANCODE_KP_DIVIDE:
        return "[DIVIDE]"sv;
    case SDL_SCANCODE_KP_MULTIPLY:
        return "[MULT]"sv;
    case SDL_SCANCODE_KP_MINUS:
        return "[MINUS]"sv;
    case SDL_SCANCODE_KP_PLUS:
        return "[PLUS]"sv;
    case SDL_SCANCODE_KP_ENTER:
        return "[ENTER]"sv;
    case SDL_SCANCODE_KP_1:
        return "[Num1]"sv;
    case SDL_SCANCODE_KP_2:
        return "[Num2]"sv;
    case SDL_SCANCODE_KP_3:
        return "[Num3]"sv;
    case SDL_SCANCODE_KP_4:
        return "[Num4]"sv;
    case SDL_SCANCODE_KP_5:
        return "[Num5]"sv;
    case SDL_SCANCODE_KP_6:
        return "[Num6]"sv;
    case SDL_SCANCODE_KP_7:
        return "[Num7]"sv;
    case SDL_SCANCODE_KP_8:
        return "[Num8]"sv;
    case SDL_SCANCODE_KP_9:
        return "[Num9]"sv;
    case SDL_SCANCODE_KP_0:
        return "[Num0]"sv;
    case SDL_SCANCODE_KP_PERIOD:
        return "[PERIOD]"sv;
    case SDL_SCANCODE_KP_EQUALS:
        return "[EQUALS]"sv;
    case SDL_SCANCODE_NONUSBACKSLASH:
        return "[BSLASH]"sv;
    case SDL_SCANCODE_APPLICATION:
        return "[APP]"sv;
    case SDL_SCANCODE_POWER:
        return "[POWER]"sv;
    case SDL_SCANCODE_LCTRL:
        return "[LCTRL]"sv;
    case SDL_SCANCODE_LSHIFT:
        return "[LSHIFT]"sv;
    case SDL_SCANCODE_LALT:
        return "[LALT]"sv;
    case SDL_SCANCODE_LGUI:
        return "[LGUI]"sv;
    case SDL_SCANCODE_RCTRL:
        return "[RCTRL]"sv;
    case SDL_SCANCODE_RSHIFT:
        return "[RSHIFT]"sv;
    case SDL_SCANCODE_RALT:
        return "[RALT]"sv;
    case SDL_SCANCODE_RGUI:
        return "[RGUI]"sv;
    case static_cast<SDL_Scancode>(301):
        return "[M1]"sv;
    case static_cast<SDL_Scancode>(302):
        return "[M2]"sv;
    case static_cast<SDL_Scancode>(303):
        return "[M3]"sv;
    case static_cast<SDL_Scancode>(304):
        return "[M4]"sv;
    case static_cast<SDL_Scancode>(305):
        return "[M5]"sv;
    case static_cast<SDL_Scancode>(306):
        return "[M6]"sv;
    case static_cast<SDL_Scancode>(307):
        return "[M7]"sv;
    case static_cast<SDL_Scancode>(308):
        return "[M8]"sv;
    case static_cast<SDL_Scancode>(309):
        return "[M9]"sv;
    case static_cast<SDL_Scancode>(310):
        return "[M10]"sv;
    case static_cast<SDL_Scancode>(311):
        return "[M11]"sv;
    case static_cast<SDL_Scancode>(312):
        return "[M12]"sv;
    case static_cast<SDL_Scancode>(313):
        return "[M13]"sv;
    case static_cast<SDL_Scancode>(314):
        return "[M14]"sv;
    case static_cast<SDL_Scancode>(315):
        return "[M15]"sv;
    case static_cast<SDL_Scancode>(316):
        return "[M16]"sv;
    case SDL_SCANCODE_UNKNOWN:
        return "[NONE]"sv;
    default:
        return "[???]"sv;
    }
}
void Configuration::resetInputControlsToDefault()
{
    setDefaultControls();
    gui.refreshScreens();
}
void Configuration::setDefaultControls()
{
    for (std::size_t i{}; i < static_cast<std::size_t>(InputControlType::COUNT); i++)
        setInputControlScancode(static_cast<InputControlType>(i), defaultControls[i]);
}
optCRef<EnemyData> Configuration::getSuitableEnemy() { return enemyData[mapRandom.generate() % enemyData.getSize()]; }
nlohmann::json Configuration::openJSONFile(std::string_view fileName)
{
    nlohmann::json result;

    auto configFile = std::ifstream(fileName.data(), std::ios::in | std::ios::binary);
    if (!configFile)
        return result;

    result = nlohmann::json::parse(configFile, nullptr, false);
    return result;
}
bool Configuration::loadOptions()
{
    // Try opening configuration file
    auto configJSON = openJSONFile(Constants::configFileName);
    if (configJSON.is_discarded() || !configJSON.is_object())
    {
        setDefaultControls();

        // Can't open the file, create a new one with default values
        if (!saveOptions())
            return false;

        // If still can't open, give up
        configJSON = openJSONFile(Constants::configFileName);
        if (configJSON.is_discarded() || !configJSON.is_object())
        {
            logger.logError("Couldn't open created config file, check if game folder needs admin permissions"sv);
            return false;
        }
    }

    readJSONValue(configJSON, "windowWidth"sv, windowWidth);
    readJSONValue(configJSON, "windowHeight"sv, windowHeight);
    readJSONValue(configJSON, "windowFullscreen"sv, isFullscreen);
    readJSONValue(configJSON, "windowMaximized"sv, isMaximized);

    auto readInputControl = [this, &configJSON](std::string_view optionName, InputControlType type)
    {
        SDL_Scancode scancode{};
        readJSONValue(configJSON, optionName, scancode);
        setInputControlScancode(type, scancode);
    };
    readInputControl("controlMoveUp"sv, InputControlType::eMoveUp);
    readInputControl("controlMoveDown"sv, InputControlType::eMoveDown);
    readInputControl("controlMoveLeft"sv, InputControlType::eMoveLeft);
    readInputControl("controlMoveRight"sv, InputControlType::eMoveRight);
    readInputControl("controlAttack"sv, InputControlType::eAttack);
    readInputControl("controlPause"sv, InputControlType::ePause);
    readInputControl("controlSearch"sv, InputControlType::eSearch);
    readInputControl("controlDiscoveries"sv, InputControlType::eDiscoveries);
    readInputControl("controlDebug"sv, InputControlType::eDebug);
    readInputControl("controlStopTime"sv, InputControlType::eStopTime);
    readInputControl("controlStepTime"sv, InputControlType::eStepTime);

    return true;
}
bool Configuration::saveOptions()
{
    nlohmann::json configJSON;
    configJSON["windowWidth"sv] = windowWidth;
    configJSON["windowHeight"sv] = windowHeight;
    configJSON["windowFullscreen"sv] = isFullscreen;
    configJSON["windowMaximized"sv] = isMaximized;

    configJSON["controlMoveUp"sv] = inputControlToScancode[InputControlType::eMoveUp];
    configJSON["controlMoveDown"sv] = inputControlToScancode[InputControlType::eMoveDown];
    configJSON["controlMoveLeft"sv] = inputControlToScancode[InputControlType::eMoveLeft];
    configJSON["controlMoveRight"sv] = inputControlToScancode[InputControlType::eMoveRight];
    configJSON["controlAttack"sv] = inputControlToScancode[InputControlType::eAttack];
    configJSON["controlPause"sv] = inputControlToScancode[InputControlType::ePause];
    configJSON["controlSearch"sv] = inputControlToScancode[InputControlType::eSearch];
    configJSON["controlDiscoveries"sv] = inputControlToScancode[InputControlType::eDiscoveries];
    configJSON["controlDebug"sv] = inputControlToScancode[InputControlType::eDebug];
    configJSON["controlStopTime"sv] = inputControlToScancode[InputControlType::eStopTime];
    configJSON["controlStepTime"sv] = inputControlToScancode[InputControlType::eStepTime];

    std::ofstream configFile(Constants::configFileName.data(), std::ios::out | std::ios::binary);
    if (!configFile)
    {
        logger.logError("Couldn't create config file, check if game folder needs admin permissions"sv);
        return false;
    }

    configFile << std::setw(4) << configJSON << std::endl;
    return true;
}
void Configuration::loadData()
{
    auto dataJSON = openJSONFile(Constants::dataFileName);
    if (dataJSON.is_discarded() || !dataJSON.is_object())
        return;

    if (dataJSON.contains("enemies") && dataJSON["enemies"].is_array())
    {
        auto const& enemyArrayJSON = dataJSON["enemies"];
        if (enemyArrayJSON.size() > enemyData.getCapacity())
            logger.logInfo("Too many enemy types in config"sv);

        for (std::size_t i{}; i < std::min(enemyArrayJSON.size(), enemyData.getCapacity()); i++)
        {
            auto const& enemyJSON = enemyArrayJSON[i];

            EnemyData data;
            readJSONValue(enemyJSON, "name"sv, data.name);
            readJSONValue(enemyJSON, "symbol"sv, data.symbol);
            readJSONValue(enemyJSON, "color"sv, data.color);
            readJSONValue(enemyJSON, "speed"sv, data.speed);
            readJSONValue(enemyJSON, "mass"sv, data.mass);
            readJSONValue(enemyJSON, "weaponColor"sv, data.weaponColor);
            readJSONValue(enemyJSON, "damage"sv, data.damage);
            readJSONValue(enemyJSON, "attackTime"sv, data.attackTime);

            FixedString<16> weaponType;
            readJSONValue(enemyJSON, "weaponType"sv, weaponType);
            data.weaponType = weaponType == "dagger"sv ? WeaponType::eDagger : weaponType == "club"sv ? WeaponType::eClub : WeaponType::eClaw;

            enemyData.emplaceBack(data);
        }
    }
}
void Configuration::setInputControlScancode(InputControlType type, SDL_Scancode scancode)
{
    if (auto oldScancode = inputControlToScancode[type]; oldScancode != SDL_SCANCODE_UNKNOWN)
        scancodeToInputControl[oldScancode] = InputControlType::COUNT;

    if (auto oldControl = scancodeToInputControl[scancode]; oldControl != InputControlType::COUNT)
        inputControlToScancode[oldControl] = SDL_SCANCODE_UNKNOWN;

    scancodeToInputControl[scancode] = type;
    inputControlToScancode[type] = scancode;

    gui.refreshScreens();
}
template <class Value>
void Configuration::readJSONValue(nlohmann::json const& json, std::string_view key, Value& value)
{
    if (!json.contains(key))
    {
        logger.logInfo("Requested key not found in JSON"sv);
        return;
    }

    auto const& jsonValue = json[key];

    using ValueType = std::decay_t<decltype(value)>;

    if constexpr (std::is_same_v<ValueType, std::uint8_t>)
    {
        if (!jsonValue.is_string() || jsonValue.size() != 1)
        {
            logger.logInfo("Requested JSON value was not a char"sv);
            return;
        }

        value = jsonValue.get<std::string>()[0];
    }
    else if constexpr (IsLikeStringView<ValueType>)
    {
        if (!jsonValue.is_string())
        {
            logger.logInfo("Requested JSON value was not a string"sv);
            return;
        }

        auto str = jsonValue.get<std::string>();
        if (str.size() > value.getCapacity())
        {
            logger.logInfo("Requested JSON string is too big for storage"sv);
            str.resize(value.getCapacity());
        }

        value = str;
    }
    else if constexpr (std::is_same_v<ValueType, Color>)
    {
        if (!jsonValue.is_array() || jsonValue.size() != 4)
        {
            logger.logInfo("Requested JSON value was not a color array"sv);
            return;
        }

        for (std::uint64_t i = 0; i < 4; i++)
        {
            if (!jsonValue[i].is_number_integer())
            {
                logger.logInfo("Requested JSON value inside a color array was not an integer"sv);
                return;
            }
        }

        value.r = jsonValue[0].get<std::uint8_t>();
        value.g = jsonValue[1].get<std::uint8_t>();
        value.b = jsonValue[2].get<std::uint8_t>();
        value.a = jsonValue[3].get<std::uint8_t>();
    }
    else if constexpr (std::is_same_v<ValueType, bool>)
    {
        if (!json[key].is_boolean())
        {
            logger.logInfo("Requested JSON value was not a boolean"sv);
            return;
        }

        value = json[key].get<bool>();
    }
    else if constexpr (std::is_integral_v<ValueType>)
    {
        if (!json[key].is_number_integer())
        {
            logger.logInfo("Requested JSON value was not an integer"sv);
            return;
        }

        value = json[key].get<ValueType>();
    }
    else if constexpr (std::is_floating_point_v<ValueType>)
    {
        if (!json[key].is_number())
        {
            logger.logInfo("Requested JSON value was not a number"sv);
            return;
        }

        value = json[key].get<ValueType>();
    }
    else if constexpr (std::is_enum_v<ValueType>)
    {
        if (!json[key].is_number_unsigned())
        {
            logger.logInfo("Requested JSON value was not an enum"sv);
            return;
        }

        value = static_cast<ValueType>(json[key].get<std::size_t>());
    }
    else
    {
        logger.logInfo("Requested JSON value of unknown type"sv);
    }
}
/*
 * Map implementation
 */
Map::Tile::Tile(std::int64_t x, std::int64_t y, Type type) : type(type)
{
    quad.init(QuadData{{Constants::mapOffset + x + 0.5f, y + 0.5f}, {Color::pack(0, 0, 0, 255), Color::pack(0, 0, 0, 255)}, info[(std::size_t)type].glyph});
}
void Map::init()
{
    for (std::int64_t y = 0; y < Constants::mapHeight; y++)
    {
        for (std::int64_t x = 0; x < Constants::mapWidth; x++)
        {
            getTile(x, y) = Tile(x, y, Tile::Type::eWall);
        }
    }

    generateLevel();

    updateVisibilityFunc = &Map::updateVisibility;
    (this->*updateVisibilityFunc)(0.0);
    enemyHandler.populateLevel();
}
void Map::update()
{
    (this->*updateVisibilityFunc)(0.0);

    for (auto& item : items)
        item.update();
}
void Map::updateDraw(double deltaTime)
{
    debugLines.clear();
    (this->*updateVisibilityFunc)(deltaTime);

    for (auto& item : items)
        item.updateDraw(deltaTime);
}
std::optional<Item> Map::pickupItem(std::int64_t x, std::int64_t y, bool onlyGold)
{
    for (std::size_t i = 0; i < items.getSize(); i++)
    {
        auto [itemX, itemY] = items[i].getPosition();
        if ((std::int64_t)itemX != x || (std::int64_t)itemY != y)
            continue;

        if (items[i].getType() != Item::Type::eGold && onlyGold)
            continue;

        items[i].setVisible(false);
        std::optional<Item> result(std::move(items[i]));
        items.erase(i);
        return result;
    }

    return std::nullopt;
}
Map::Room const& Map::getRandomRoom() const { return levelData.rooms[mapRandom.generate() % levelData.roomCount]; }
FixedVector<std::pair<std::int64_t, std::int64_t>, 32> Map::getPath(std::int64_t startX, std::int64_t startY, std::int64_t endX, std::int64_t endY) const
{
    logger.extraAssert(startX >= 0 && startX < Constants::mapWidth && startY >= 0 && startY < Constants::mapHeight && endX >= 0 && endX < Constants::mapWidth &&
                           endY >= 0 && endY < Constants::mapHeight,
                       "Requested tile path out of bounds"sv);

    FixedVector<std::pair<std::int64_t, std::int64_t>, 32> result;

    struct VisitedData
    {
        std::int64_t x{};
        std::int64_t y{};
        std::int64_t previousIndex{};
    };
    FixedVector<VisitedData, 32> visitedTiles;

    auto distance = (endX - startX) * (endX - startX) + (endY - startY) * (endY - startY);
    struct AStarData
    {
        std::int64_t x{};
        std::int64_t y{};
        std::int64_t index{};
        std::int64_t currentScore{};
        std::int64_t predictedScore{};
    };
    FixedVector<AStarData, 32> searchTiles;

    visitedTiles.emplaceBack(startX, startY, 0);
    searchTiles.emplaceBack(startX, startY, 0, 0, distance);

    while (!searchTiles.isEmpty())
    {
        auto minScore = std::numeric_limits<std::int64_t>::max();
        std::int64_t tileIndex{};
        for (std::int64_t i = 0; i < searchTiles.getSize(); i++)
        {
            if (searchTiles[i].predictedScore < minScore)
            {
                tileIndex = i;
                minScore = searchTiles[i].predictedScore;
            }
        }

        auto currentTile = searchTiles[tileIndex];
        searchTiles.erase(tileIndex);

        auto checkNeighbor = [this, &visitedTiles, &searchTiles, &currentTile, &result, endX, endY](std::int64_t x, std::int64_t y)
        {
            if (getTileSolid(x, y))
                return;

            if (x == endX && y == endY)
            {
                std::int64_t visitedIndex{currentTile.index};
                FixedVector<std::int64_t, 32> visitedIndices;
                while (visitedTiles[visitedIndex].previousIndex != 0)
                {
                    visitedIndices.emplaceBack(visitedIndex);
                    visitedIndex = visitedTiles[visitedIndex].previousIndex;
                }
                visitedIndices.emplaceBack(visitedIndex);

                for (std::int64_t i = visitedIndices.getSize() - 1; i >= 0; i--)
                {
                    auto index = visitedIndices[i];
                    result.emplaceBack(visitedTiles[index].x, visitedTiles[index].y);
                }
                result.emplaceBack(endX, endY);
                return;
            }

            std::int64_t neighborIndex{-1};
            for (std::int64_t i = 0; i < visitedTiles.getSize(); i++)
            {
                if (visitedTiles[i].x == x && visitedTiles[i].y == y)
                    neighborIndex = i;
            }

            if (neighborIndex == -1)
            {
                neighborIndex = visitedTiles.getSize();
                visitedTiles.emplaceBack(x, y, currentTile.index);
                auto neighborDistance = (endX - x) * (endX - x) + (endY - y) * (endY - y);
                searchTiles.emplaceBack(x, y, neighborIndex, currentTile.currentScore + 1, neighborDistance);
            }
        };
        checkNeighbor(currentTile.x + 1, currentTile.y);
        if (visitedTiles.isFull() || !result.isEmpty())
            break;
        checkNeighbor(currentTile.x - 1, currentTile.y);
        if (visitedTiles.isFull() || !result.isEmpty())
            break;
        checkNeighbor(currentTile.x, currentTile.y + 1);
        if (visitedTiles.isFull() || !result.isEmpty())
            break;
        checkNeighbor(currentTile.x, currentTile.y - 1);
        if (visitedTiles.isFull() || !result.isEmpty())
            break;
    }

    return result;
}
bool Map::getTileSolid(std::int64_t x, std::int64_t y) const
{
    logger.extraAssert(x >= 0 && x < Constants::mapWidth && y >= 0 && y < Constants::mapHeight, "Requested is tile solid out of bounds"sv);

    return getTile(x, y).getIsSolid();
}
bool Map::getTileOpaque(std::int64_t x, std::int64_t y) const
{
    logger.extraAssert(x >= 0 && x < Constants::mapWidth && y >= 0 && y < Constants::mapHeight, "Requested is tile opaque out of bounds"sv);

    return getTile(x, y).getIsOpaque();
}
bool Map::getTileFloor(std::int64_t x, std::int64_t y) const
{
    logger.extraAssert(x >= 0 && x < Constants::mapWidth && y >= 0 && y < Constants::mapHeight, "Requested is tile a floor out of bounds"sv);

    return getTile(x, y).getIsFloor();
}
double Map::getTileBrightness(std::int64_t x, std::int64_t y) const
{
    logger.extraAssert(x >= 0 && x < Constants::mapWidth && y >= 0 && y < Constants::mapHeight, "Requested tile brightness out of bounds"sv);

    return tileBrightnessMask[x + Constants::mapWidth * y];
}
bool Map::getTileInLineOfSight(std::int64_t x, std::int64_t y) const
{
    logger.extraAssert(x >= 0 && x < Constants::mapWidth && y >= 0 && y < Constants::mapHeight, "Requested is tile in line of sight out of bounds"sv);

    return tileBrightnessMask[x + Constants::mapWidth * y] >= Constants::mapInLineOfSightValue;
}
void Map::generateLevel()
{
    for (std::int64_t x = 0; x < Constants::mapWidth; x++)
    {
        getTile(x, 0).setType(Tile::Type::eBedrock);
        levelData.tilesOccupiedMask[x] = true;

        getTile(x, Constants::mapHeight - 1).setType(Tile::Type::eBedrock);
        levelData.tilesOccupiedMask[x + (Constants::mapHeight - 1) * Constants::mapWidth] = true;
    }

    for (std::int64_t y = 0; y < Constants::mapHeight; y++)
    {
        getTile(0, y).setType(Tile::Type::eBedrock);
        levelData.tilesOccupiedMask[y * Constants::mapWidth] = true;

        getTile(Constants::mapWidth - 1, y).setType(Tile::Type::eBedrock);
        levelData.tilesOccupiedMask[Constants::mapWidth - 1 + y * Constants::mapWidth] = true;
    }

    auto tryPlacingRoom = [this](std::int64_t doorX, std::int64_t doorY, std::int64_t originX, std::int64_t originY, std::int64_t width, std::int64_t height,
                                 std::uint64_t originDirection)
    {
        for (auto y = originY; y < originY + height; y++)
        {
            for (auto x = originX; x < originX + width; x++)
            {
                if (x < 0 || x >= Constants::mapWidth || y < 0 || y >= Constants::mapHeight || levelData.tilesOccupiedMask[x + y * Constants::mapWidth])
                    return;
            }
        }

        for (auto y = originY; y < originY + height; y++)
        {
            for (auto x = originX; x < originX + width; x++)
                getTile(x, y).setType(Tile::Type::eFloor);
        }

        for (auto y = originY - 1; y < originY + height + 1; y++)
        {
            for (auto x = originX - 1; x < originX + width + 1; x++)
                levelData.tilesOccupiedMask[x + y * Constants::mapWidth] = true;
        }

        std::uint64_t value = std::random_device()() % 2;
        getTile(doorX, doorY).setType(value == 0 ? Tile::Type::eDoor : Tile::Type::eFloor);

        levelData.rooms[levelData.roomCount].originX = originX;
        levelData.rooms[levelData.roomCount].originY = originY;
        levelData.rooms[levelData.roomCount].width = width;
        levelData.rooms[levelData.roomCount].height = height;
        levelData.roomCount++;

        if (originDirection != 1)
        {
            for (auto y = originY; y < originY + height; y++)
            {
                std::int64_t x = originX - 1;
                if (x < 2)
                    break;

                if (getTileFloor(x - 1, y) && getTileFloor(x + 1, y))
                {
                    std::uint64_t value = std::random_device()() % 2;
                    getTile(x, y).setType(value == 0 ? Tile::Type::eDoor : Tile::Type::eFloor);
                    break;
                }
            }
        }

        if (originDirection != 3)
        {
            for (auto y = originY; y < originY + height; y++)
            {
                std::int64_t x = originX + width;
                if (x >= Constants::mapWidth - 2)
                    break;

                if (getTileFloor(x - 1, y) && getTileFloor(x + 1, y))
                {
                    std::uint64_t value = std::random_device()() % 2;
                    getTile(x, y).setType(value == 0 ? Tile::Type::eDoor : Tile::Type::eFloor);
                    break;
                }
            }
        }

        if (originDirection != 2)
        {
            for (auto x = originX; x < originX + width; x++)
            {
                std::int64_t y = originY - 1;
                if (y < 2)
                    break;

                if (getTileFloor(x, y - 1) && getTileFloor(x, y + 1))
                {
                    std::uint64_t value = std::random_device()() % 2;
                    getTile(x, y).setType(value == 0 ? Tile::Type::eDoor : Tile::Type::eFloor);
                    break;
                }
            }
        }

        if (originDirection != 0)
        {
            for (auto x = originX; x < originX + width; x++)
            {
                std::int64_t y = originY + height;
                if (y >= Constants::mapHeight - 2)
                    break;

                if (getTileFloor(x, y - 1) && getTileFloor(x, y + 1))
                {
                    std::uint64_t value = std::random_device()() % 2;
                    getTile(x, y).setType(value == 0 ? Tile::Type::eDoor : Tile::Type::eFloor);
                    break;
                }
            }
        }
    };

    auto& startingRoom = levelData.rooms[0];
    startingRoom.originX = 30;
    startingRoom.originY = 29;
    startingRoom.width = 20;
    startingRoom.height = 5;
    levelData.roomCount = 1;
    for (auto y = startingRoom.originY; y < startingRoom.originY + startingRoom.height; y++)
    {
        for (auto x = startingRoom.originX; x < startingRoom.originX + startingRoom.width; x++)
            getTile(x, y).setType(Tile::Type::eFloor);
    }
    for (auto y = startingRoom.originY - 1; y < startingRoom.originY + startingRoom.height + 1; y++)
    {
        for (auto x = startingRoom.originX - 1; x < startingRoom.originX + startingRoom.width + 1; x++)
            levelData.tilesOccupiedMask[x + y * Constants::mapWidth] = true;
    }
    getTile(40, 34).setType(Tile::Type::eExit);

    for (std::int64_t i = 0; i < 200; i++)
    {
    outerLoop:
        std::int64_t currentWidthSpread = std::llround(18 * (1.0 - i / 200.0)) + 1;
        std::int64_t currentHeightSpread = std::llround(8 * (1.0 - i / 200.0)) + 1;

        auto randomness = std::random_device();
        auto const& originRoom = levelData.rooms[randomness() % levelData.roomCount];

        std::int64_t newRoomWidth = 2ll + randomness() % currentWidthSpread;
        std::int64_t newRoomHeight = 2ll + randomness() % currentHeightSpread;

        std::uint64_t newDirection = randomness() % 4;
        if (newDirection == 0)
        {
            std::int64_t doorX = originRoom.originX + randomness() % originRoom.width;
            std::int64_t doorY = originRoom.originY - 1;

            std::int64_t newRoomOriginX = doorX - randomness() % newRoomWidth;
            std::int64_t newRoomOriginY = doorY - newRoomHeight;

            tryPlacingRoom(doorX, doorY, newRoomOriginX, newRoomOriginY, newRoomWidth, newRoomHeight, newDirection);
        }
        else if (newDirection == 1)
        {
            std::int64_t doorX = originRoom.originX + originRoom.width;
            std::int64_t doorY = originRoom.originY + randomness() % originRoom.height;

            std::int64_t newRoomOriginX = doorX + 1;
            std::int64_t newRoomOriginY = doorY - randomness() % newRoomHeight;

            tryPlacingRoom(doorX, doorY, newRoomOriginX, newRoomOriginY, newRoomWidth, newRoomHeight, newDirection);
        }
        else if (newDirection == 2)
        {
            std::int64_t doorX = originRoom.originX + randomness() % originRoom.width;
            std::int64_t doorY = originRoom.originY + originRoom.height;

            std::int64_t newRoomOriginX = doorX - randomness() % newRoomWidth;
            std::int64_t newRoomOriginY = doorY + 1;

            tryPlacingRoom(doorX, doorY, newRoomOriginX, newRoomOriginY, newRoomWidth, newRoomHeight, newDirection);
        }
        else if (newDirection == 3)
        {
            std::int64_t doorX = originRoom.originX - 1;
            std::int64_t doorY = originRoom.originY + randomness() % originRoom.height;

            std::int64_t newRoomOriginX = doorX - newRoomWidth;
            std::int64_t newRoomOriginY = doorY - randomness() % newRoomHeight;

            tryPlacingRoom(doorX, doorY, newRoomOriginX, newRoomOriginY, newRoomWidth, newRoomHeight, newDirection);
        }
    }

    for (std::int64_t i = 0; i < 20; i++)
    {
        auto const& room = getRandomRoom();
        std::int64_t spawnX = room.originX + mapRandom.generate() % room.width;
        std::int64_t spawnY = room.originY + mapRandom.generate() % room.height;

        std::int64_t itemTypeVal = mapRandom.generate() % 10;
        Item::Type itemType = itemTypeVal == 0 ? Item::Type::eFood : Item::Type::eGold;
        items.emplaceBack(itemType, spawnX + 0.5, spawnY + 0.5);
    }

    auto const& room = getRandomRoom();
    std::int64_t spawnX = room.originX + mapRandom.generate() % room.width;
    std::int64_t spawnY = room.originY + mapRandom.generate() % room.height;
    items.emplaceBack(Item::Type::eAmulet, spawnX + 0.5, spawnY + 0.5);
}
void Map::updateVisibleTile(std::int64_t x, std::int64_t y, double distanceX, double distanceY, double visionRange)
{
    std::uint64_t tileIndex = x + y * Constants::mapWidth;
    double distance2 = distanceX * distanceX / 4.0 + distanceY * distanceY;
    double visionRange2 = visionRange * visionRange;
    if (distance2 <= visionRange2)
    {
        // Light strength falls off quadratically down to a minimum
        double lightStrength = 1.0 - distance2 / visionRange2 * (1.0 - Constants::mapMinBrightness);
        getTile(x, y).updateDraw(lightStrength);
        tileBrightnessMask[tileIndex] = lightStrength;
        lastVisibleTiles.emplaceBack(tileIndex);
    }
    else
    {
        // Tiles outside the vision range are marked with special value
        tileBrightnessMask[tileIndex] = Constants::mapInLineOfSightValue;
    }
}
void Map::updateVisibility(double deltaTime)
{
    for (auto tileIndex : lastVisibleTiles)
    {
        tiles[tileIndex].updateDraw(Constants::mapInLineOfSightValue);
        tileBrightnessMask[tileIndex] = 0.0;
    }
    lastVisibleTiles.clear();

    auto [playerX, playerY] = player.getPredictedPosition(deltaTime);

    double visionRange = 8.0;
    std::int64_t lookupRange = std::ceil(visionRange);

    std::int64_t playerCellX = playerX;
    std::int64_t playerCellY = playerY;
    updateVisibleTile(playerX, playerY, playerCellX + 0.5 - playerX, playerCellY + 0.5 - playerY, visionRange);

    // Get initial left slopes
    double topStartSlope{}, bottomStartSlope{};
    std::int64_t startCellX{playerCellX - 1};
    while (true)
    {
        updateVisibleTile(startCellX, playerCellY, startCellX + 0.5 - playerX, playerCellY + 0.5 - playerY, visionRange);

        if (getTileOpaque(startCellX, playerCellY))
        {
            topStartSlope = (playerCellY - playerY) / (startCellX + 1 - playerX);
            bottomStartSlope = (playerCellY + 1 - playerY) / (startCellX + 1 - playerX);

            break;
        }
        startCellX--;
    }

    // Get initial right slopes
    double topEndSlope{}, bottomEndSlope{};
    std::int64_t endCellX{playerCellX + 1};
    while (true)
    {
        updateVisibleTile(endCellX, playerCellY, endCellX + 0.5 - playerX, playerCellY + 0.5 - playerY, visionRange);

        if (getTileOpaque(endCellX, playerCellY))
        {
            topEndSlope = (playerCellY - playerY) / (endCellX - playerX);
            bottomEndSlope = (playerCellY + 1 - playerY) / (endCellX - playerX);

            break;
        }
        endCellX++;
    }

    calculateVisibilitySector(playerCellY - 1, -1, topStartSlope, startCellX + 1.0, startCellX, topEndSlope, endCellX, endCellX, playerX, playerY, visionRange);
    calculateVisibilitySector(playerCellY + 1, 1, bottomStartSlope, startCellX + 1.0, startCellX, bottomEndSlope, endCellX, endCellX, playerX, playerY,
                              visionRange);
}
void Map::updateVisibilityDebug(double deltaTime)
{
    for (auto tileIndex : lastVisibleTiles)
    {
        tiles[tileIndex].updateDraw(Constants::mapInLineOfSightValue);
        tileBrightnessMask[tileIndex] = 0.0;
    }
    lastVisibleTiles.clear();

    auto [playerX, playerY] = player.getPredictedPosition(deltaTime);

    double visionRange = 8.0;
    std::int64_t lookupRange = std::ceil(visionRange);

    std::int64_t playerCellX = playerX;
    std::int64_t playerCellY = playerY;
    updateVisibleTile(playerX, playerY, playerCellX + 0.5 - playerX, playerCellY + 0.5 - playerY, visionRange);

    // Get initial left slopes
    double topStartSlope{}, bottomStartSlope{};
    std::int64_t startCellX{playerCellX - 1};
    while (true)
    {
        updateVisibleTile(startCellX, playerCellY, startCellX + 0.5 - playerX, playerCellY + 0.5 - playerY, visionRange);

        if (getTileOpaque(startCellX, playerCellY))
        {
            topStartSlope = (playerCellY - playerY) / (startCellX + 1 - playerX);
            bottomStartSlope = (playerCellY + 1 - playerY) / (startCellX + 1 - playerX);

            debugLines.emplaceBack(Constants::mapOffset + playerX, playerY, Constants::mapOffset + startCellX + 1, playerY);
            break;
        }
        startCellX--;
    }

    // Get initial right slopes
    double topEndSlope{}, bottomEndSlope{};
    std::int64_t endCellX{playerCellX + 1};
    while (true)
    {
        updateVisibleTile(endCellX, playerCellY, endCellX + 0.5 - playerX, playerCellY + 0.5 - playerY, visionRange);

        if (getTileOpaque(endCellX, playerCellY))
        {
            topEndSlope = (playerCellY - playerY) / (endCellX - playerX);
            bottomEndSlope = (playerCellY + 1 - playerY) / (endCellX - playerX);

            debugLines.emplaceBack(Constants::mapOffset + playerX, playerY, Constants::mapOffset + endCellX, playerY);
            break;
        }
        endCellX++;
    }

    calculateVisibilitySectorDebug(playerCellY - 1, -1, topStartSlope, startCellX + 1.0, startCellX, topEndSlope, endCellX, endCellX, playerX, playerY,
                                   visionRange);
    calculateVisibilitySectorDebug(playerCellY + 1, 1, bottomStartSlope, startCellX + 1.0, startCellX, bottomEndSlope, endCellX, endCellX, playerX, playerY,
                                   visionRange);
}
void Map::calculateVisibilitySector(std::int64_t currentCellY, std::int64_t directionY, double startSlope, double startEnterX, std::int64_t startCellX,
                                    double endSlope, double endEnterX, std::int64_t endCellX, double playerX, double playerY, double visionRange)
{
    auto getSlopesCorrect = [](double startSlope, double endSlope)
    { return (startSlope < 0.0 && endSlope > 0.0) || (endSlope < startSlope && (endSlope > 0.0 || startSlope < 0.0)); };

    while (true)
    {
        // Check if left entry cell is opaque
        std::int64_t currentStartCellX{};
        if (getTileOpaque(startCellX, currentCellY))
        {
            // Move right until first non-opaque cell found
            do
            {
                updateVisibleTile(startCellX, currentCellY, startCellX + 0.5 - playerX, currentCellY + 0.5 - playerY, visionRange);

                // Terminate if no more empty cells
                if (startCellX == endCellX)
                    return;

                startCellX++;
            }
            while (getTileOpaque(startCellX, currentCellY));
            updateVisibleTile(startCellX, currentCellY, startCellX + 0.5 - playerX, currentCellY + 0.5 - playerY, visionRange);

            currentStartCellX = startCellX + 1;

            if (startCellX < playerX)
            {
                // New slope is towards top right corner of last blocking cell in top half and towards bottom right corner in bottom half
                startSlope = (currentCellY + (directionY == 1) - playerY) / (startCellX - playerX);
                startEnterX = startCellX;
                startCellX--;
            }
            else
            {
                // New slope is towards bottom right corner of last blocking cell in top half and towards top right corner in bottom half
                startSlope = (currentCellY + (directionY == -1) - playerY) / (startCellX - playerX);
                startEnterX = startCellX + 1.0 / std::abs(startSlope);
                startCellX = (std::int64_t)startEnterX;
            }
        }
        else
        {
            currentStartCellX = startCellX + 1;
            updateVisibleTile(startCellX, currentCellY, startCellX + 0.5 - playerX, currentCellY + 0.5 - playerY, visionRange);

            // Move left until first opaque cell or move right until next row
            double directedStartSlope = startSlope * directionY;
            if (directedStartSlope < 0.0)
            {
                double initialEnterX = startEnterX;
                startEnterX += 1.0 / directedStartSlope;
                startCellX = startEnterX;
                for (auto i = currentStartCellX - 2; i >= startCellX; i--)
                {
                    updateVisibleTile(i, currentCellY, i + 0.5 - playerX, currentCellY + 0.5 - playerY, visionRange);
                    if (getTileOpaque(i, currentCellY))
                    {
                        // New slope is towards top right corner of first opaque cell in top half and towards bottom right corner in bottom half
                        startSlope = (currentCellY + (directionY == 1) - playerY) / (i + 1 - playerX);
                        startEnterX = i + 1;
                        startCellX = i;
                        break;
                    }
                }
            }
            else
            {
                startEnterX = startEnterX + 1.0 / directedStartSlope;
                startCellX = startEnterX;
            }
        }

        std::int64_t currentEndCellX{endCellX};
        double directedEndSlope = endSlope * directionY;
        if (directedEndSlope > 0.0)
        {
            if (getTileOpaque(endCellX, currentCellY))
            {
                do
                {
                    updateVisibleTile(endCellX, currentCellY, endCellX + 0.5 - playerX, currentCellY + 0.5 - playerY, visionRange);
                    endCellX--;
                }
                while (getTileOpaque(endCellX, currentCellY));

                currentEndCellX = endCellX;
                endCellX++;

                if (endCellX > playerX)
                {
                    endSlope = (currentCellY + (directionY == 1) - playerY) / (endCellX - playerX);
                    endEnterX = endCellX;
                }
                else
                {
                    endSlope = (currentCellY + (directionY == -1) - playerY) / (endCellX - playerX);
                    endEnterX = endCellX + 1.0 / endSlope * directionY;
                    endCellX = (std::int64_t)endEnterX;
                }
            }
            else
            {
                updateVisibleTile(currentEndCellX, currentCellY, currentEndCellX + 0.5 - playerX, currentCellY + 0.5 - playerY, visionRange);
                double initialEndEnterX = endEnterX;
                endEnterX = endEnterX + 1.0 / directedEndSlope;
                endCellX = (std::int64_t)(endEnterX);
                for (auto i = currentEndCellX + 1; i <= endCellX; i++)
                {
                    updateVisibleTile(i, currentCellY, i + 0.5 - playerX, currentCellY + 0.5 - playerY, visionRange);
                    if (getTileOpaque(i, currentCellY))
                    {
                        endSlope = (currentCellY + (directionY == 1) - playerY) / (i - playerX);
                        endEnterX = i;
                        endCellX = i;
                    }
                }
            }
        }
        else
        {
            endEnterX = endEnterX + 1.0 / directedEndSlope;
            endCellX = (std::int64_t)(endEnterX);
        }

        for (auto i = currentStartCellX; i <= currentEndCellX; i++)
        {
            if (getTileOpaque(i, currentCellY))
            {
                double newEndSlope =
                    i < playerX ? (currentCellY + (directionY == -1) - playerY) / (i - playerX) : (currentCellY + (directionY == 1) - playerY) / (i - playerX);
                if (getSlopesCorrect(startSlope * directionY, newEndSlope * directionY))
                {
                    if (newEndSlope * directionY < 0.0)
                    {
                        double newEndEnterX = i + 1.0 / newEndSlope * directionY;
                        std::int64_t newEndCellX = newEndEnterX;

                        calculateVisibilitySector(currentCellY + directionY, directionY, startSlope, startEnterX, startCellX, newEndSlope, newEndEnterX,
                                                  newEndCellX, playerX, playerY, visionRange);
                    }
                    else
                    {
                        calculateVisibilitySector(currentCellY + directionY, directionY, startSlope, startEnterX, startCellX, newEndSlope, i, i, playerX,
                                                  playerY, visionRange);
                    }
                }

                do
                {
                    updateVisibleTile(i, currentCellY, i + 0.5 - playerX, currentCellY + 0.5 - playerY, visionRange);

                    if (i == currentEndCellX)
                        return;
                    i++;
                }
                while (getTileOpaque(i, currentCellY));

                startSlope =
                    i < playerX ? (currentCellY + (directionY == 1) - playerY) / (i - playerX) : (currentCellY + (directionY == -1) - playerY) / (i - playerX);
                double directedStartSlope = startSlope * directionY;
                if (directedStartSlope < 0.0)
                {
                    startEnterX = i;
                    startCellX = i - 1;
                }
                else
                {
                    startEnterX = i + 1.0 / directedStartSlope;
                    startCellX = startEnterX;
                }
            }

            updateVisibleTile(i, currentCellY, i + 0.5 - playerX, currentCellY + 0.5 - playerY, visionRange);
        }

        if (!getSlopesCorrect(startSlope * directionY, endSlope * directionY))
            return;

        currentCellY += directionY;
    }
}
void Map::calculateVisibilitySectorDebug(std::int64_t currentCellY, std::int64_t directionY, double startSlope, double startEnterX, std::int64_t startCellX,
                                         double endSlope, double endEnterX, std::int64_t endCellX, double playerX, double playerY, double visionRange)
{
    auto getSlopesCorrect = [](double startSlope, double endSlope)
    { return (startSlope < 0.0 && endSlope > 0.0) || (endSlope < startSlope && (endSlope > 0.0 || startSlope < 0.0)); };

    while (true)
    {
        // Check if left entry cell is opaque
        std::int64_t currentStartCellX{};
        if (getTileOpaque(startCellX, currentCellY))
        {
            debugLines.emplaceBack(Constants::mapOffset + playerX, playerY, Constants::mapOffset + startEnterX, currentCellY + (directionY == -1));

            // Move right until first non-opaque cell found
            do
            {
                updateVisibleTile(startCellX, currentCellY, startCellX + 0.5 - playerX, currentCellY + 0.5 - playerY, visionRange);

                // Terminate if no more empty cells
                if (startCellX == endCellX)
                {
                    debugLines.emplaceBack(Constants::mapOffset + playerX, playerY, Constants::mapOffset + endEnterX, currentCellY + (directionY == -1));
                    return;
                }
                startCellX++;
            }
            while (getTileOpaque(startCellX, currentCellY));
            updateVisibleTile(startCellX, currentCellY, startCellX + 0.5 - playerX, currentCellY + 0.5 - playerY, visionRange);

            currentStartCellX = startCellX + 1;

            if (startCellX < playerX)
            {
                // New slope is towards top right corner of last blocking cell in top half and towards bottom right corner in bottom half
                startSlope = (currentCellY + (directionY == 1) - playerY) / (startCellX - playerX);
                startEnterX = startCellX;
                startCellX--;
            }
            else
            {
                // New slope is towards bottom right corner of last blocking cell in top half and towards top right corner in bottom half
                startSlope = (currentCellY + (directionY == -1) - playerY) / (startCellX - playerX);
                startEnterX = startCellX + 1.0 / std::abs(startSlope);
                startCellX = (std::int64_t)startEnterX;
            }
        }
        else
        {
            currentStartCellX = startCellX + 1;
            updateVisibleTile(startCellX, currentCellY, startCellX + 0.5 - playerX, currentCellY + 0.5 - playerY, visionRange);

            // Move left until first opaque cell or move right until next row
            double directedStartSlope = startSlope * directionY;
            if (directedStartSlope < 0.0)
            {
                double initialEnterX = startEnterX;
                startEnterX += 1.0 / directedStartSlope;
                startCellX = startEnterX;
                for (auto i = currentStartCellX - 2; i >= startCellX; i--)
                {
                    updateVisibleTile(i, currentCellY, i + 0.5 - playerX, currentCellY + 0.5 - playerY, visionRange);
                    if (getTileOpaque(i, currentCellY))
                    {
                        debugLines.emplaceBack(Constants::mapOffset + playerX, playerY, Constants::mapOffset + i + 1,
                                               currentCellY + (directionY == -1) - (initialEnterX - i - 1) * startSlope);

                        // New slope is towards top right corner of first opaque cell in top half and towards bottom right corner in bottom half
                        startSlope = (currentCellY + (directionY == 1) - playerY) / (i + 1 - playerX);
                        startEnterX = i + 1;
                        startCellX = i;
                        break;
                    }
                }
            }
            else
            {
                startEnterX = startEnterX + 1.0 / directedStartSlope;
                startCellX = startEnterX;
            }
        }

        std::int64_t currentEndCellX{endCellX};
        double directedEndSlope = endSlope * directionY;
        if (directedEndSlope > 0.0)
        {
            if (getTileOpaque(endCellX, currentCellY))
            {
                debugLines.emplaceBack(Constants::mapOffset + playerX, playerY, Constants::mapOffset + endEnterX, currentCellY + (directionY == -1));

                do
                {
                    updateVisibleTile(endCellX, currentCellY, endCellX + 0.5 - playerX, currentCellY + 0.5 - playerY, visionRange);
                    endCellX--;
                }
                while (getTileOpaque(endCellX, currentCellY));

                currentEndCellX = endCellX;
                endCellX++;

                if (endCellX > playerX)
                {
                    endSlope = (currentCellY + (directionY == 1) - playerY) / (endCellX - playerX);
                    endEnterX = endCellX;
                }
                else
                {
                    endSlope = (currentCellY + (directionY == -1) - playerY) / (endCellX - playerX);
                    endEnterX = endCellX + 1.0 / endSlope * directionY;
                    endCellX = (std::int64_t)endEnterX;
                }
            }
            else
            {
                updateVisibleTile(currentEndCellX, currentCellY, currentEndCellX + 0.5 - playerX, currentCellY + 0.5 - playerY, visionRange);
                double initialEndEnterX = endEnterX;
                endEnterX = endEnterX + 1.0 / directedEndSlope;
                endCellX = (std::int64_t)(endEnterX);
                for (auto i = currentEndCellX + 1; i <= endCellX; i++)
                {
                    updateVisibleTile(i, currentCellY, i + 0.5 - playerX, currentCellY + 0.5 - playerY, visionRange);
                    if (getTileOpaque(i, currentCellY))
                    {
                        debugLines.emplaceBack(Constants::mapOffset + playerX, playerY, Constants::mapOffset + i,
                                               currentCellY + (directionY == -1) + (i - initialEndEnterX) * endSlope);

                        endSlope = (currentCellY + (directionY == 1) - playerY) / (i - playerX);
                        endEnterX = i;
                        endCellX = i;
                    }
                }
            }
        }
        else
        {
            endEnterX = endEnterX + 1.0 / directedEndSlope;
            endCellX = (std::int64_t)(endEnterX);
        }

        for (auto i = currentStartCellX; i <= currentEndCellX; i++)
        {
            if (getTileOpaque(i, currentCellY))
            {
                double newEndSlope =
                    i < playerX ? (currentCellY + (directionY == -1) - playerY) / (i - playerX) : (currentCellY + (directionY == 1) - playerY) / (i - playerX);
                if (getSlopesCorrect(startSlope * directionY, newEndSlope * directionY))
                {
                    if (newEndSlope * directionY < 0.0)
                    {
                        double newEndEnterX = i + 1.0 / newEndSlope * directionY;
                        std::int64_t newEndCellX = newEndEnterX;

                        calculateVisibilitySectorDebug(currentCellY + directionY, directionY, startSlope, startEnterX, startCellX, newEndSlope, newEndEnterX,
                                                       newEndCellX, playerX, playerY, visionRange);
                    }
                    else
                        calculateVisibilitySectorDebug(currentCellY + directionY, directionY, startSlope, startEnterX, startCellX, newEndSlope, i, i, playerX,
                                                       playerY, visionRange);
                }

                do
                {
                    updateVisibleTile(i, currentCellY, i + 0.5 - playerX, currentCellY + 0.5 - playerY, visionRange);

                    if (i == currentEndCellX)
                        return;
                    i++;
                }
                while (getTileOpaque(i, currentCellY));

                startSlope =
                    i < playerX ? (currentCellY + (directionY == 1) - playerY) / (i - playerX) : (currentCellY + (directionY == -1) - playerY) / (i - playerX);
                double directedStartSlope = startSlope * directionY;
                if (directedStartSlope < 0.0)
                {
                    startEnterX = i;
                    startCellX = i - 1;
                }
                else
                {
                    startEnterX = i + 1.0 / directedStartSlope;
                    startCellX = startEnterX;
                }
            }

            updateVisibleTile(i, currentCellY, i + 0.5 - playerX, currentCellY + 0.5 - playerY, visionRange);
        }

        if (!getSlopesCorrect(startSlope * directionY, endSlope * directionY))
            return;

        currentCellY += directionY;
    }
}
/*
 * Player implementation
 */
Player::Player(double velocity) : PhysicsComponent(40.5, 33.5, 0.4, 0.4, 0.32, 0.4)
{
    auto [x, y] = getPosition();
    quadReference.init(QuadData{{Constants::mapOffset + x, y}, {Color::pack(64, 255, 0, 255), Color::pack(64, 255, 0, 0)}, 64});

    setMaxVelocity(velocity);
    setHealth(100);

    weapon.init(WeaponType::eDagger, Color(255, 255, 0, 255), 1, 0.25, true);
    gui.setInventory(inventory, gold);
}
void Player::onMousePressed(std::uint32_t x, std::uint32_t y)
{
    auto [positionX, positionY] = getPosition();
    weapon.startAttack(positionX, positionY, x + 0.5, y + 0.5);
}
void Player::update()
{
    PhysicsComponent::update();

    auto [positionX, positionY] = getPosition();
    weapon.update(positionX, positionY);

    if ((std::int64_t)positionX != lastTileX || (std::int64_t)positionY != lastTileY)
    {
        lastTileX = (std::int64_t)positionX;
        lastTileY = (std::int64_t)positionY;

        if (auto itemOpt = map.pickupItem(lastTileX, lastTileY, inventory.isFull()))
        {
            if (itemOpt->getType() == Item::Type::eGold)
                gold++;
            else
            {
                if (itemOpt->getType() == Item::Type::eAmulet)
                    hasAmulet = true;

                inventory.emplaceBack(std::move(*itemOpt));
            }

            gui.setInventory(inventory, gold);
        }

        if (hasAmulet && lastTileX == 40 && lastTileY == 33)
            gui.showGameOver(true);
    }
}
void Player::updateDraw(double deltaTime)
{
    float guiOffset = 48.0f;

    auto [x, y] = getPredictedPosition(deltaTime);
    quadReference.setPosition(guiOffset + x, y);
    weapon.updateDraw(guiOffset + x, y);
}
void Player::takeDamage(std::int64_t damage)
{
    if (health == 0)
        return;

    std::int64_t newHealth = health - damage;
    if (newHealth < 0)
        newHealth = 0;

    setHealth(newHealth);
}
void Player::setMovement(std::int64_t movementX, std::int64_t movementY)
{
    if (health == 0)
        return;

    setMovementDirection(movementX, movementY);
}
void Player::setHealth(std::int64_t newHealth)
{
    health = newHealth;

    gui.setPlayerHealth(health / 100.0);
    if (health == 0)
    {
        gui.showGameOver(false);
        setMovementDirection(0.0, 0.0);
    }
}
/*
 * EnemyHandler implementation
 */
EnemyHandler::Enemy::Enemy(EnemyData const& data, double positionX, double positionY, State initialState, bool isDrawDebug) :
    PhysicsComponent(positionX, positionY, 0.45, 0.45, 0.45, 0.45), color(data.color), state(initialState)
{
    auto [x, y] = getPosition();
    quad.init(QuadData{{Constants::mapOffset + x, y}, {color.getPacked(), color.getTransparentPacked()}, data.symbol});

    if (isDrawDebug)
        updateDrawDebug();

    setMass(data.mass);
    setMaxVelocity(data.speed);

    weapon.init(data.weaponType, data.weaponColor, data.damage, data.attackTime, false);
}
bool EnemyHandler::spawnEnemy()
{
    auto enemyDataOpt = configuration.getSuitableEnemy();
    if (!enemyDataOpt)
        return false;

    auto const& spawnRoom = map.getRandomRoom();
    std::int64_t spawnX = spawnRoom.originX + mapRandom.generate() % spawnRoom.width;
    std::int64_t spawnY = spawnRoom.originY + mapRandom.generate() % spawnRoom.height;

    if (map.getTileInLineOfSight(spawnX, spawnY))
        return false;

    enemies.emplaceBack(*enemyDataOpt, spawnX + 0.5, spawnY + 0.5, Enemy::State::eSleeping, isDrawDebug);

    return true;
}
void EnemyHandler::Enemy::update(double playerX, double playerY, double playerVelocityX, double playerVelocityY, std::int64_t stealthRange)
{
    auto [x, y] = getPosition();

    double distanceX = (playerX - x) / 2.0;
    double distanceY = playerY - y;
    double totalDistance = std::sqrt(distanceX * distanceX + distanceY * distanceY);

    auto moveTowards = [this, x, y](double targetX, double targetY)
    {
        double distanceX = (targetX - x) / 2.0;
        double distanceY = targetY - y;
        setMovementDirection(distanceX, distanceY);
    };

    if (state == State::eHunting)
    {
        if (path.getBack().first != (std::int64_t)playerX || path.getBack().second != (std::int64_t)playerY)
            setPathTo(playerX, playerY);

        auto [targetTileX, targetTileY] = path[currentPathIndex];

        if (std::abs(targetTileX + 0.5 - x) > 1.0 || std::abs(targetTileY + 0.5 - y) > 1.0)
        {
            moveTowards(targetTileX + 0.5, targetTileY + 0.5);
        }
        else
        {
            currentPathIndex++;
            if (currentPathIndex >= path.getSize())
            {
                setState(State::eWandering);
                setMovementDirection(0.0, 0.0);
            }
        }
    }
    else if (state == State::eSleeping)
    {
        if (totalDistance < stealthRange && map.getTileInLineOfSight(x, y))
        {
            stealthTimer += Constants::tickDuration;
            if (stealthTimer > lastCheckedStealthTime + 0.5)
            {
                lastCheckedStealthTime += 0.5;
                std::uint64_t detectRoll = mapRandom.generate() % 4;
                if (detectRoll == 0)
                {
                    setState(State::eHunting);
                    stealthTimer = 0.0;
                    lastCheckedStealthTime = 0.0;
                    setPathTo(playerX, playerY);
                }
            }
        }
    }
    else if (state == State::eWandering)
    {
        if (totalDistance < stealthRange && map.getTileInLineOfSight(x, y))
        {
            if ((std::int64_t)x != (std::int64_t)playerX || (std::int64_t)y != (std::int64_t)playerY)
            {
                setState(State::eHunting);
                setPathTo(playerX, playerY);
            }
        }
    }

    PhysicsComponent::update();

    std::tie(x, y) = getPosition();
    weapon.update(x, y);
}
void EnemyHandler::Enemy::updateDraw(double deltaTime)
{
    auto [x, y] = getPosition();
    auto [vx, vy] = getVelocity();
    quad.setPosition(Constants::mapOffset + x + vx * deltaTime, y + vy * deltaTime);

    if (enemyHandler.isDrawDebug)
        stateQuad.setPosition(Constants::mapOffset + x + vx * deltaTime - 0.25, y + vy * deltaTime + 0.25);

    auto brightness = map.getTileBrightness(x, y);
    if (brightness < Constants::mapMinBrightness)
    {
        quad.setColor(0);
        quad.setBackgroundColor(0);
    }
    else
    {
        quad.setColor(Color::pack(color.r * brightness, color.g * brightness, color.b * brightness, color.a));
        quad.setBackgroundColor(Color::pack(color.r * brightness, color.g * brightness, color.b * brightness, 0));

        weapon.updateDraw(Constants::mapOffset + x, y);
    }
}
void EnemyHandler::Enemy::updateDrawDebug()
{
    pathQuads.clear();
    stateQuad = {};
    if (enemyHandler.isDrawDebug)
    {
        for (auto [pathX, pathY] : path)
        {
            pathQuads.emplaceBack();
            pathQuads.getBack().init(QuadData{{Constants::mapOffset + pathX + 0.5, pathY + 0.5}, {Color::pack(0, 0, 0, 0), Color::pack(255, 0, 0, 128)}, ' '});
        }

        auto [x, y] = getPosition();
        QuadData stateData{{Constants::mapOffset + x - 0.25, y + 0.25}, {Color::pack(255, 0, 0, 255), Color::pack(255, 0, 0, 0)}, 'S'};
        stateData.setScale(0.5, 0.5);
        stateQuad.init(stateData);
        stateQuad.setGlyph(state == State::eSleeping ? 'S' : state == State::eWandering ? 'W' : state == State::eHunting ? 'H' : '?');
    }
}
void EnemyHandler::Enemy::setState(State newState)
{
    state = newState;
    if (enemyHandler.isDrawDebug)
    {
        stateQuad.setGlyph(state == State::eSleeping ? 'S' : state == State::eWandering ? 'W' : state == State::eHunting ? 'H' : '?');
    }
}
void EnemyHandler::Enemy::setPathTo(std::int64_t x, std::int64_t y)
{
    auto [currentX, currentY] = getPosition();

    path = map.getPath(currentX, currentY, x, y);
    currentPathIndex = 0;

    if (path.isEmpty())
        path.emplaceBack(currentX, currentY);

    updateDrawDebug();
}
void EnemyHandler::update()
{
    currentTime += Constants::tickDuration;

    if (currentTime - lastEnemySpawnTime > 60.0)
    {
        spawnEnemy();

        lastEnemySpawnTime = currentTime;
    }

    auto [playerX, playerY] = player.getPosition();
    auto [playerVelocityX, playerVelocityY] = player.getVelocity();
    auto stealthRange = player.getStealthRange();
    for (auto& enemy : enemies)
        enemy.update(playerX, playerY, playerVelocityX, playerVelocityY, stealthRange);
}
void EnemyHandler::updateDraw(double deltaTime)
{
    for (auto& enemy : enemies)
        enemy.updateDraw(deltaTime);
}
void EnemyHandler::inflictDamage(double damageX, double damageY)
{
    for (std::size_t i = 0; i < enemies.getSize(); i++)
    {
        auto [enemyX, enemyY] = enemies[i].getPosition();
        if (damageX > enemyX - 0.6 && damageX < enemyX + 0.6 && damageY > enemyY - 0.6 && damageY < enemyY + 0.6)
        {
            enemies.erase(i);
            i--;
        }
    }
}
void EnemyHandler::populateLevel()
{
    for (std::int64_t i = 0; i < 20; i++)
    {
        if (!spawnEnemy())
            i--;
    }
}
void EnemyHandler::setDrawDebug(bool draw)
{
    enemyHandler.isDrawDebug = draw;
    for (auto& enemy : enemies)
        enemy.updateDrawDebug();
}
/*
 * Game implementation
 */
bool Game::init()
{
    if (!logger.init())
        return false;

    if (!configuration.init())
        return false;

    if (!renderWindow.init(Constants::appName, Constants::appVersion, Constants::appIdentifier, Constants::appCreator, Constants::appCopyright,
                           Constants::appType, configuration.getWindowWidth(), configuration.getWindowHeight(),
                           configuration.getIsFullscreen()      ? RenderWindow::State::eFullscreen
                               : configuration.getIsMaximized() ? RenderWindow::State::eMaximized
                                                                : RenderWindow::State::eWindowed))
        return false;

    if (!renderEngine.init("Abrogue"sv, 0, 1, 0))
        return false;

    gui.init();

    currentTimeNS = SDL_GetTicksNS();
    lastFPSLogTimeNS = currentTimeNS;

    return true;
}
bool Game::update()
{
    auto newTimeNS = SDL_GetTicksNS();
    auto deltaTimeNS = newTimeNS - currentTimeNS;
    if (deltaTimeNS > maxFrameTimeNS)
        maxFrameTimeNS = deltaTimeNS;
    gameDeltaTimeNS += deltaTimeNS;
    guiDeltaTimeNS += deltaTimeNS;
    currentTimeNS = newTimeNS;

    std::uint64_t updateCount{};
    while (guiDeltaTimeNS >= Constants::tickDurationNS)
    {
        gui.update();

        guiDeltaTimeNS -= Constants::tickDurationNS;

        updateCount++;
        if (updateCount > 4)
        {
            guiDeltaTimeNS = 0;
            logger.logInfo("Can't keep up with gui, skipping ticks");
            break;
        }
    }
    gui.updateDraw(guiDeltaTimeNS / 1.e9);

    updateCount = 0;
    while (gameDeltaTimeNS >= adjustedTickDuration)
    {
        advanceStep();

        gameDeltaTimeNS -= adjustedTickDuration;

        updateCount++;
        if (updateCount > 4)
        {
            gameDeltaTimeNS = 0;
            logger.logInfo("Can't keep up with game, skipping ticks");
            break;
        }
    }
    if (!updateDraw(gameDeltaTimeNS / 1.e9 * speedPercentage / 100))
        return false;

    framesDrawn++;
    if (auto timeSinceLastLog = currentTimeNS - lastFPSLogTimeNS; timeSinceLastLog >= 1'000'000'000)
    {
        auto fps = framesDrawn * 10'000'000'000 / timeSinceLastLog;
        fps = fps / 10 + (fps % 10 >= 5);
        gui.setFPS(fps, timeSinceLastLog / maxFrameTimeNS);

        framesDrawn = 0;
        maxFrameTimeNS = 1;
        lastFPSLogTimeNS = currentTimeNS;
    }

    return true;
}
void Game::advanceStep() const
{
    if (state != State::eRunning)
        return;

    player.update();
    map.update();
    enemyHandler.update();
}
void Game::startGame()
{
    currentTimeNS = SDL_GetTicksNS();
    mapRandom.seed(currentTimeNS);
    visualRandom.seed(currentTimeNS);

    player = Player(10.0);
    map.init();

    state = State::eRunning;

    gui.showPlayArea();
}
void Game::quitToDesktop() { state = State::eFinished; }
void Game::setPaused(bool paused) { state = paused ? State::ePaused : State::eRunning; }
void Game::setSpeedPercentage(std::uint64_t speed)
{
    logger.extraAssert(speed <= 1'000'000, "Set incorrect speed multiplier");

    speedPercentage = speed;
    gameDeltaTimeNS = 0;
    adjustedTickDuration = speedPercentage != 0 ? Constants::tickDurationNS * 100 / speedPercentage : std::numeric_limits<std::uint64_t>::max();
}
void Game::setPlayerMovement(std::int64_t movementX, std::int64_t movementY) const
{
    if (state != State::eRunning && state != State::ePaused)
        return;

    player.setMovement(movementX, movementY);
}
void Game::quitToMainMenu()
{
    gui.showMainMenu();

    state = State::eNotStarted;
    map = Map();
    player = Player();
    enemyHandler = EnemyHandler();
}
void Game::refreshWindowState() const
{
    gui.refreshScreens();
    configuration.updateWindowOptions();
}
bool Game::updateDraw(double deltaTime) const
{
    if (state == State::eRunning)
    {
        player.updateDraw(deltaTime);

        map.updateDraw(deltaTime);

        enemyHandler.updateDraw(deltaTime);
    }

    return renderEngine.drawFrame();
}


/*
 * PauseMenu implementation
 */
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

    if (type == eResume)
        gui.showPlayArea();
    else if (type == eOptions)
        gui.showOptionsMenu();
    else if (type == eAbandonGame || type == eSaveAndQuit)
        game.quitToMainMenu();
}
/*
 * MainMenu implementation
 */
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

    if (type == eStartGame)
        game.startGame();
    else if (type == eOptions)
        gui.showOptionsMenu();
    else if (type == eQuitToDesktop)
        game.quitToDesktop();
}
/*
 * PlayArea implementation
 */
void PlayArea::init()
{
    using enum ButtonType;

    buttons[ePause].init(""sv, 26, 1);
    buttons[eHealth].init("       Health       "sv, 0, 1);
    buttons[eHealth].setBackgroundColor(Constants::healthBackgroundColor, Constants::healthHoverColor);
    buttons[eNutrition].init("     Nutrition      "sv, 0, 2);
    buttons[eNutrition].setBackgroundColor(Constants::nutritionBackgroundColor, Constants::nutritionHoverColor);
    buttons[eGold].init("Gold:0"sv, 0, 5);
    buttons[eSearch].init(""sv, 11, 35);
    buttons[eInventory].init("Inventory"sv, 0, 6);

    for (std::size_t i = 0; i < (std::size_t)eInventorySlotLast - (std::size_t)eInventorySlotFirst; i++)
        buttons[(std::size_t)eInventorySlotFirst + i].init(""sv, 0, 7 + i);

    buttons[eDepth].init("Depth:"sv, 0, 35);

    tabButtons[TabButtonType::eDebug].init(""sv, 15, 0);
    tabButtons[TabButtonType::eDiscoveries].init(""sv, 22, 35);
    tabButtons[TabButtonType::eMenu].init(""sv, 38, 35);

    refreshLabels();
}
void PlayArea::updateInventory(FixedVector<Item, 20> const& inventory, std::int64_t gold)
{
    using enum ButtonType;

    Array<char, 32> goldString{"Gold:"};
    std::to_chars(goldString.getData() + 5, goldString.getData() + 31, gold);
    buttons[eGold].setText(goldString.getData());

    for (std::size_t i = 0; i < inventory.getSize(); i++)
        buttons[(std::size_t)eInventorySlotFirst + i].setText(inventory[i].getName());

    for (std::size_t i = inventory.getSize(); i < 20; i++)
        buttons[(std::size_t)eInventorySlotFirst + i].setText(""sv);
}
void PlayArea::setPaused(bool paused)
{
    using enum ButtonType;

    if (buttons[ePause].getPressed() == paused)
        return;

    FixedString<16> pauseText;

    buttons[ePause].setPressed(paused);
    buttons[ePause].setText(pauseText.fill(paused ? "PAUSED"sv : "Pause"sv, configuration.getInputControlName(InputControlType::ePause)));
    game.setPaused(paused);
}
void PlayArea::setPlayerHealth(double percentage) { buttons[ButtonType::eHealth].setProgress(percentage); }
void PlayArea::onButtonPressed(ButtonType type)
{
    using enum ButtonType;

    if (type == ePause)
        setPaused(!getPaused());
}
void PlayArea::onTabButtonPressed(TabButtonType type) const
{
    using enum TabButtonType;

    if (type == eMenu)
        gui.onPauseMenuHotkeyPressed();
    else if (type == eDiscoveries)
        gui.onDiscoveriesHotkeyPressed();
    else if (type == eDebug)
        gui.onDebugHotkeyPressed();
}
void PlayArea::refreshLabels()
{
    using enum ButtonType;

    FixedString<32> labelText;

    buttons[ePause].setText(labelText.fill(buttons[ePause].getPressed() ? "PAUSED"sv : "Pause"sv, configuration.getInputControlName(InputControlType::ePause)));
    buttons[eSearch].setText(labelText.fill("Search"sv, configuration.getInputControlName(InputControlType::eSearch)));
    tabButtons[TabButtonType::eDebug].setText(labelText.fill("Debug"sv, configuration.getInputControlName(InputControlType::eDebug)));
    tabButtons[TabButtonType::eDiscoveries].setText(labelText.fill("Discoveries"sv, configuration.getInputControlName(InputControlType::eDiscoveries)));
    tabButtons[TabButtonType::eMenu].setText(labelText.fill("Menu[ESC]"sv));
}
/*
 * GameOver implementation
 */
void GameOver::init()
{
    labels[LabelType::eGameOver].init(""sv, 58, 15);
    labels[LabelType::ePressSpace].init(""sv, 52, 16);

    refreshLabels();
}
void GameOver::setWinner(bool winner)
{
    labels[LabelType::eGameOver].setPosition(winner ? 49 : 58, 15);
    labels[LabelType::eGameOver].setText(winner ? "Number One Victory Royale"sv : "joever"sv);
}
void GameOver::refreshLabels()
{
    FixedString<32> labelText;

    labels[LabelType::ePressSpace].setText(labelText.fill("Press [ESC] to quit"sv));
}
/*
 * DebugMenu implementation
 */
void DebugMenu::init()
{
    using enum ButtonType;

    buttons[eTitle].init("Debug Options"sv, 56, 6);
    buttons[eStopTime].init(""sv, 40, 8);
    buttons[eStepTime].init(""sv, 40, 10);
    buttons[eShowDamage].init("Show damage"sv, 70, 8);
    buttons[eShowViewcone].init("Show viewcone"sv, 70, 10);
    buttons[eShowEnemies].init("Show enemies"sv, 70, 12);

    refreshLabels();
}
void DebugMenu::onButtonPressed(ButtonType type)
{
    using enum ButtonType;

    if (type == eStopTime)
    {
        buttons[eStopTime].togglePressed();
        game.setSpeedPercentage(buttons[eStopTime].getPressed() ? 0 : 100);
    }
    else if (type == eStepTime && buttons[eStopTime].getPressed())
    {
        game.advanceStep();
    }
    else if (type == eShowDamage)
    {
        buttons[eShowDamage].togglePressed();
        Weapon::setDrawDebug(buttons[eShowDamage].getPressed());
    }
    else if (type == eShowViewcone)
    {
        buttons[eShowViewcone].togglePressed();
        Map::setDrawDebugViewcone(buttons[eShowViewcone].getPressed());
    }
    else if (type == eShowEnemies)
    {
        buttons[eShowEnemies].togglePressed();
        enemyHandler.setDrawDebug(buttons[eShowEnemies].getPressed());
    }
}
void DebugMenu::resetToDefault()
{
    using enum ButtonType;

    if (buttons[eStopTime].getPressed())
        onButtonPressed(eStopTime);

    if (buttons[eShowDamage].getPressed())
        onButtonPressed(eShowDamage);

    if (buttons[eShowViewcone].getPressed())
        onButtonPressed(eShowViewcone);
}
void DebugMenu::refreshLabels()
{
    using enum ButtonType;

    FixedString<32> labelText;

    buttons[eStopTime].setText(labelText.fill("Stop time"sv, configuration.getInputControlName(InputControlType::eStopTime)));
    buttons[eStepTime].setText(labelText.fill("Step time"sv, configuration.getInputControlName(InputControlType::eStepTime)));
}
/*
 * OptionsMenu implementation
 */
void OptionsMenu::init()
{
    using enum ButtonType;

    labels[LabelType::eControls].init("Controls"sv, 60, 5);
    labels[LabelType::eVideo].init("Video"sv, 82, 5);
    labels[LabelType::eResolution].init(""sv, 80, 7);

    buttons[eMoveUp].init(""sv, 58, 7);
    buttons[eMoveDown].init(""sv, 58, 9);
    buttons[eMoveLeft].init(""sv, 58, 11);
    buttons[eMoveRight].init(""sv, 58, 13);
    buttons[eAttack].init(""sv, 58, 15);
    buttons[ePause].init(""sv, 58, 17);
    buttons[eSearch].init(""sv, 58, 19);
    buttons[eDiscoveries].init(""sv, 58, 21);
    buttons[eDebug].init(""sv, 58, 23);
    buttons[eStopTime].init(""sv, 58, 25);
    buttons[eStepTime].init(""sv, 58, 27);

    buttons[eResetToDefault].init("Reset To Default"sv, 58, 32);

    buttons[eFullscreen].init(""sv, 78, 9);

    refreshLabels();
}
void OptionsMenu::onButtonPressed(ButtonType type)
{
    using enum ButtonType;

    if (type <= eStepTime)
        buttons[type].setPressed(true);

    if (type == eMoveUp)
        inputHandler.setChangingControlType(InputControlType::eMoveUp);
    else if (type == eMoveDown)
        inputHandler.setChangingControlType(InputControlType::eMoveDown);
    else if (type == eMoveLeft)
        inputHandler.setChangingControlType(InputControlType::eMoveLeft);
    else if (type == eMoveRight)
        inputHandler.setChangingControlType(InputControlType::eMoveRight);
    else if (type == eAttack)
        inputHandler.setChangingControlType(InputControlType::eAttack);
    else if (type == ePause)
        inputHandler.setChangingControlType(InputControlType::ePause);
    else if (type == eSearch)
        inputHandler.setChangingControlType(InputControlType::eSearch);
    else if (type == eDiscoveries)
        inputHandler.setChangingControlType(InputControlType::eDiscoveries);
    else if (type == eDebug)
        inputHandler.setChangingControlType(InputControlType::eDebug);
    else if (type == eStopTime)
        inputHandler.setChangingControlType(InputControlType::eStopTime);
    else if (type == eStepTime)
        inputHandler.setChangingControlType(InputControlType::eStepTime);
    else if (type == eResetToDefault)
        configuration.resetInputControlsToDefault();
    else if (type == eFullscreen)
    {
        auto isFullscreen = configuration.getIsFullscreen();
        if (isFullscreen)
            renderWindow.setIsMaximized(configuration.getIsMaximized());

        renderWindow.setIsFullscreen(!isFullscreen);
    }
}
void OptionsMenu::refreshLabels()
{
    using enum ButtonType;

    FixedString<32> labelText;

    buttons[eMoveUp].setText(labelText.fill("Move Up"sv, configuration.getInputControlName(InputControlType::eMoveUp)));
    buttons[eMoveDown].setText(labelText.fill("Move Down"sv, configuration.getInputControlName(InputControlType::eMoveDown)));
    buttons[eMoveLeft].setText(labelText.fill("Move Left"sv, configuration.getInputControlName(InputControlType::eMoveLeft)));
    buttons[eMoveRight].setText(labelText.fill("Move Right"sv, configuration.getInputControlName(InputControlType::eMoveRight)));
    buttons[eAttack].setText(labelText.fill("Attack"sv, configuration.getInputControlName(InputControlType::eAttack)));
    buttons[ePause].setText(labelText.fill("Pause"sv, configuration.getInputControlName(InputControlType::ePause)));
    buttons[eSearch].setText(labelText.fill("Search"sv, configuration.getInputControlName(InputControlType::eSearch)));
    buttons[eDiscoveries].setText(labelText.fill("Discoveries"sv, configuration.getInputControlName(InputControlType::eDiscoveries)));
    buttons[eDebug].setText(labelText.fill("Debug"sv, configuration.getInputControlName(InputControlType::eDebug)));
    buttons[eStopTime].setText(labelText.fill("Stop Time"sv, configuration.getInputControlName(InputControlType::eStopTime)));
    buttons[eStepTime].setText(labelText.fill("Step Time"sv, configuration.getInputControlName(InputControlType::eStepTime)));

    auto [windowWidth, windowHeight] = renderWindow.getWindowSize();
    labels[LabelType::eResolution].setText(labelText.format("{}x{}", windowWidth, windowHeight));
    buttons[eFullscreen].setText(renderWindow.getIsFullscreen() ? "[X]Fullscreen"sv : "[ ]Fullscreen"sv);

    for (auto i = (std::size_t)eMoveUp; i <= (std::size_t)eStepTime; i++)
        buttons[i].setPressed(false);
}
/*
 * GUI implementation
 */
void GUI::init()
{
    // Initialize all available screens
    mainMenu.init();
    optionsMenu.init();
    playArea.init();
    pauseMenu.init();
    discoveries.init();
    debugMenu.init();
    gameOver.init();

    fpsLabel.init("FPS:"sv, 0, 0, true);

    setCurrentScreen(ScreenType::eMainMenu);
}
void GUI::showPlayArea() { setCurrentScreen(ScreenType::ePlayArea); }
void GUI::showMainMenu()
{
    // Restore default GUI state
    playArea.setPaused(false);
    debugMenu.resetToDefault();

    setCurrentScreen(ScreenType::eMainMenu);
}
void GUI::showOptionsMenu() { setCurrentScreen(ScreenType::eOptionsMenu); }
void GUI::showGameOver(bool winner)
{
    gameOver.setWinner(winner);

    setCurrentScreen(ScreenType::eGameOver);
}
void GUI::update()
{
    fpsLabel.update();
    popupBackground.update();
    mainMenu.update();
    optionsMenu.update();
    playArea.update();
    pauseMenu.update();
    discoveries.update();
    debugMenu.update();
    gameOver.update();
}
void GUI::updateDraw(double deltaTime)
{
    fpsLabel.updateDraw(deltaTime);
    popupBackground.updateDraw(deltaTime);
    mainMenu.updateDraw(deltaTime);
    optionsMenu.updateDraw(deltaTime);
    playArea.updateDraw(deltaTime);
    pauseMenu.updateDraw(deltaTime);
    discoveries.updateDraw(deltaTime);
    debugMenu.updateDraw(deltaTime);
    gameOver.updateDraw(deltaTime);
}
void GUI::onMouseMoved(std::int64_t x, std::int64_t y)
{
    executeOnScreen(activeScreenType, [x, y](auto& screen) { screen.updateMouseMoved(x, y); });
}
void GUI::onMousePressed(std::int64_t x, std::int64_t y)
{
    // Handle player actions when pressing on the map
    if (activeScreenType == ScreenType::ePlayArea && !playArea.getPaused() && x >= Constants::mapOffset)
        player.onMousePressed(x - Constants::mapOffset, y);

    executeOnScreen(activeScreenType, [x, y](auto& screen) { screen.updateMousePressed(x, y); });
}
void GUI::onPauseMenuHotkeyPressed()
{
    // Requesting the pause menu before the game starts does nothing
    if (activeScreenType == ScreenType::eMainMenu)
        return;

    // Requesting the pause menu after game over returns to the main menu
    if (activeScreenType == ScreenType::eGameOver)
    {
        game.quitToMainMenu();
        return;
    }

    // Close a popup if it's the current screen
    if (isScreenAPopup(activeScreenType))
    {
        playArea.setPaused(previouslyPaused);
        playArea.setTabButtonPressed(PlayArea::TabButtonType::COUNT);
        setCurrentScreen(backgroundScreenType);
        return;
    }

    // Show the pause menu
    playArea.setPaused(true);
    playArea.setTabButtonPressed(PlayArea::TabButtonType::eMenu);
    setCurrentScreen(ScreenType::ePauseMenu);
}
void GUI::onDebugHotkeyPressed()
{
    // Requesting the debug menu only works during play
    if (activeScreenType == ScreenType::eMainMenu || activeScreenType == ScreenType::ePauseMenu || activeScreenType == ScreenType::eGameOver)
        return;

    // Close debug menu if its already open
    if (activeScreenType == ScreenType::eDebugMenu)
    {
        playArea.setPaused(previouslyPaused);
        playArea.setTabButtonPressed(PlayArea::TabButtonType::COUNT);
        setCurrentScreen(backgroundScreenType);
        return;
    }

    // Show debug options
    playArea.setPaused(true);
    playArea.setTabButtonPressed(PlayArea::TabButtonType::eDebug);
    setCurrentScreen(ScreenType::eDebugMenu);
}
void GUI::onDiscoveriesHotkeyPressed()
{
    // Requesting the discoveries menu only works during play
    if (activeScreenType == ScreenType::eMainMenu || activeScreenType == ScreenType::ePauseMenu || activeScreenType == ScreenType::eGameOver)
        return;

    // Close discoveries menu if its already open
    if (activeScreenType == ScreenType::eDiscoveries)
    {
        playArea.setPaused(previouslyPaused);
        playArea.setTabButtonPressed(PlayArea::TabButtonType::COUNT);
        setCurrentScreen(backgroundScreenType);
        return;
    }

    // Show discoveries menu
    playArea.setPaused(true);
    playArea.setTabButtonPressed(PlayArea::TabButtonType::eDiscoveries);
    setCurrentScreen(ScreenType::eDiscoveries);
}
void GUI::onPauseHotkeyPressed()
{
    // Pausing only allowed during active play
    if (activeScreenType != ScreenType::ePlayArea)
        return;

    previouslyPaused = !playArea.getPaused();
    playArea.setPaused(previouslyPaused);
}
void GUI::onStopTimeHotkeyPressed()
{
    // Stopping time not allowed in main menu or during game over
    if (activeScreenType == ScreenType::eMainMenu || activeScreenType == ScreenType::eGameOver)
        return;

    debugMenu.pressButton(DebugMenu::ButtonType::eStopTime);
}
void GUI::onStepTimeHotkeyPressed()
{
    // Stepping time not allowed in main menu or during game over
    if (activeScreenType == ScreenType::eMainMenu || activeScreenType == ScreenType::eGameOver)
        return;

    debugMenu.pressButton(DebugMenu::ButtonType::eStepTime);
}
void GUI::refreshScreens()
{
    optionsMenu.refreshLabels();
    debugMenu.refreshLabels();
    gameOver.refreshLabels();
    playArea.refreshLabels();
}
void GUI::setCurrentScreen(ScreenType screenType)
{
    // Do nothing if the old and new screens are the same
    if (activeScreenType == screenType)
        return;

    auto isOldScreenAPopup = isScreenAPopup(activeScreenType);
    auto isNewScreenAPopup = isScreenAPopup(screenType);

    // Show popup on top of the already visible screen
    // If the old screen was a popup too, simply switch to the new one
    // Else keep the old screen visible in the background
    if (isNewScreenAPopup)
    {
        if (isOldScreenAPopup)
            setScreenVisible(activeScreenType, false);
        else
        {
            popupBackground.setVisible(true);
            backgroundScreenType = activeScreenType;
        }

        setScreenVisible(screenType, true);
    }
    // Hide the popup and its background
    // If the new and background screens are different, hide the old background too
    else if (isOldScreenAPopup)
    {
        popupBackground.setVisible(false);

        setScreenVisible(activeScreenType, false);
        if (screenType != backgroundScreenType)
        {
            setScreenVisible(backgroundScreenType, false);
            setScreenVisible(screenType, true);
        }

        backgroundScreenType = ScreenType::eNone;
    }
    // Hide the old screen and show the new one
    else
    {
        setScreenVisible(activeScreenType, false);
        setScreenVisible(screenType, true);
    }

    activeScreenType = screenType;

    // Refresh mouse position for the newly shown screen
    auto [x, y] = inputHandler.getMousePosition();
    onMouseMoved(x, y);
}
void GUI::setScreenVisible(ScreenType screenType, bool visible)
{
    executeOnScreen(screenType, [visible](auto& screen) { screen.setVisible(visible); });
}
bool GUI::isScreenAPopup(ScreenType screenType) const
{
    return screenType == ScreenType::ePauseMenu || screenType == ScreenType::eDiscoveries || screenType == ScreenType::eDebugMenu ||
        screenType == ScreenType::eGameOver || screenType == ScreenType::eOptionsMenu;
}
void GUI::setFPS(std::int64_t fps, std::int64_t minFPS)
{
    Array<char, 32> buf{"FPS:"};
    auto index = std::to_chars(buf.getData() + 4, buf.getData() + 14, fps).ptr - buf.getData();
    buf[index] = '/';
    std::to_chars(buf.getData() + index + 1, buf.getData() + index + 10, minFPS);

    fpsLabel.setText(buf.getData());
}
void GUI::setPlayerHealth(double percentage) { playArea.setPlayerHealth(percentage); }
void GUI::setInventory(FixedVector<Item, 20> const& inventory, std::int64_t gold) { playArea.updateInventory(inventory, gold); }
