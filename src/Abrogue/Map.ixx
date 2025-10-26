module Abrogue:Map;

import :Item;
import :DebugLine;

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

        bool getIsSolid() const
        {
            return type == Type::eBedrock || type == Type::eWall || type == Type::eExit;
        }

        bool getIsOpaque() const
        {
            return type == Type::eBedrock || type == Type::eWall || type == Type::eBush || type == Type::eDoor || type == Type::eExit;
        }

        bool getIsFloor() const
        {
            return type == Type::eFloor || type == Type::eGrass || type == Type::eBush;
        }

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
            Info{Color(96, 96, 96, 255), Color(8, 8, 8, 255), 250},
            Info{Color(0, 255, 0, 255), Color(8, 8, 8, 255), 34},
            Info{Color(0, 255, 0, 255), Color(8, 8, 8, 255), 237},
            Info{Color(255, 255, 255, 255), Color(32, 32, 32, 255), 35},
            Info{Color(255, 255, 255, 255), Color(16, 16, 16, 255), 35},
            Info{Color(255, 128, 0, 255), Color(192, 16, 0, 255), 43},
            Info{Color(255, 255, 255, 255), Color(16, 16, 192, 255), 234}
        };

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
    [[nodiscard]] FixedVector<std::pair<std::int64_t, std::int64_t>, 128> getPath(std::int64_t startX, std::int64_t startY, std::int64_t endX,
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
