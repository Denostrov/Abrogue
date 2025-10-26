module Abrogue:QuadPool;

import :Constants;

import GLM;

class QuadPool;

/*
 * Class for storing rendering data of a single quad
 */
class QuadData
{
public:
    QuadData() = default;
    //Create Data with position in world coordinates and color
    QuadData(glm::vec2 position, glm::uvec2 colors, std::uint32_t glyph, glm::vec2 scale = {1.0, 1.0})
        : position(position.x * Constants::tileScaleX, position.y * Constants::tileScaleY),
          scale(scale.x * Constants::tileScaleX, scale.y * Constants::tileScaleY),
          colors(colors), glyph(glyph)
    {}

    //Set position in world coordinates
    void setPosition(float positionX, float positionY) { position = {positionX * Constants::tileScaleX, positionY * Constants::tileScaleY}; }
    //Set scale in world coordinates
    void setScale(float scaleX, float scaleY) { scale = {Constants::tileScaleX * scaleX, Constants::tileScaleY * scaleY}; }
    //Set quad rotaion in radians
    void setRotation(float angle) { rotation = {std::cos(angle), std::sin(angle)}; }
    //Set quad rotation in precalculated cos and sin
    void setRotation(float cos, float sin) { rotation = {cos, sin}; }
    //Set packed glyph color
    void setColor(PackedColor packedColor) { colors[0] = packedColor; }
    //Set packed background color
    void setBackgroundColor(PackedColor packedColor) { colors[1] = packedColor; }
    //Set glyph index
    void setGlyph(std::uint32_t newGlyph) { glyph = newGlyph; }

private:
    glm::vec2 position{};           //Position in screenspace coordinates
    glm::vec2 scale{};              //Scale in screenspace coordinates
    glm::vec2 rotation{1.0f, 0.0f}; //Cos and Sin of rotation angle
    glm::uvec2 colors{};            //Packed glyph and background colors
    std::uint32_t glyph{};          //Index of drawn symbol
};

//Layers for defining draw order
enum class QuadLayer
{
    eMap,
    eItem,
    eEntity,
    eMapOverlay,
    ePopupBackground,
    ePopup,
    COUNT
};

/*
 * Class for a bidirectional reference to quad data
 */
template <QuadLayer>
class QuadReference
{
public:
    QuadReference() = default;

    //Create a reference to quad with data
    template <IsSameType<QuadData> T>
    QuadReference(T&& quadData);
    //Destroy a reference to quad if it exists
    ~QuadReference();

    //Swap reference indices and pointers in the pool
    QuadReference(QuadReference&& rhs) noexcept { *this = std::move(rhs); }
    QuadReference& operator=(QuadReference&& rhs) noexcept;

    //Create a reference if it doesn't exist and set its data
    template <IsSameType<QuadData> T>
    void setData(T&& quadData);
    //Destroy a reference if it exists
    void clearData();

    operator bool() const { return index != -1; }

    //Modify an existing reference
    void setPosition(float positionX, float positionY) const;
    void setGlyph(std::uint8_t glyph) const;
    void setColor(std::uint32_t packedColor) const;
    void setBackgroundColor(std::uint32_t packedColor) const;

private:
    //Creation and destruction helpers
    template<bool isConstructor, IsSameType<QuadData> T>
    void init(T&& quadData);
    template<bool isDestructor>
    void cleanup();

    std::int64_t index{-1}; //Index of quad data in the pool
};

/*
 * Class for handling quad data storage
 */
class QuadPool
{
    static constexpr Array<std::size_t, QuadLayer::COUNT> capacities{8192uz, 512uz, 512uz, 512uz, 512uz, 2048uz};

    template <QuadLayer layer>
    struct Storage
    {
        FixedVector<QuadData, capacities[layer]> data;
        FixedVector<QuadReference<layer>*, capacities[layer]> references;
    };

public:
    QuadPool() = default;

    template <QuadLayer layer>
    [[nodiscard]] auto getData() { return getStorage<layer>().data.getSpan(); }
    [[nodiscard]] static constexpr auto getCapacity() { return std::accumulate(capacities.begin(), capacities.end(), 0uz); }

private:
    template <QuadLayer layer>
    [[nodiscard]] auto&& getStorage() { return std::get<(std::size_t)layer>(storage); }

    EnumTupleType<QuadLayer, Storage> storage;

    template <QuadLayer layer>
    friend class QuadReference;
};
inline QuadPool quadPool;

template <QuadLayer layer>
template <IsSameType<QuadData> T>
QuadReference<layer>::QuadReference(T&& quadData)
{
    init<true>(std::forward<T>(quadData));
}

template <QuadLayer layer>
QuadReference<layer>::~QuadReference()
{
    cleanup<true>();
}

template <QuadLayer layer>
QuadReference<layer>& QuadReference<layer>::operator=(QuadReference&& rhs) noexcept
{
    auto& references = quadPool.getStorage<layer>().references;

    //Update reference pointers in quad pool
    if (index != -1)
        references[index] = &rhs;
    if (rhs.index != -1)
        references[rhs.index] = this;

    std::swap(index, rhs.index);
    return *this;
}

template <QuadLayer layer>
template <IsSameType<QuadData> T>
void QuadReference<layer>::setData(T&& quadData)
{
    init<false>(std::forward<T>(quadData));
}

template <QuadLayer layer>
void QuadReference<layer>::clearData()
{
    cleanup<false>();
}

template <QuadLayer layer>
void QuadReference<layer>::setPosition(float positionX, float positionY) const
{
    logger.extraAssert(index != -1, "Set position of invalid quad reference");

    auto& data = quadPool.getStorage<layer>().data;
    data[index].setPosition(positionX, positionY);
}

template <QuadLayer layer>
void QuadReference<layer>::setGlyph(std::uint8_t glyph) const
{
    logger.extraAssert(index != -1, "Set glyph of invalid quad reference");

    auto& data = quadPool.getStorage<layer>().data;
    data[index].setGlyph(glyph);
}

template <QuadLayer layer>
void QuadReference<layer>::setColor(std::uint32_t packedColor) const
{
    logger.extraAssert(index != -1, "Set color of invalid quad reference");

    auto& data = quadPool.getStorage<layer>().data;
    data[index].setColor(packedColor);
}

template <QuadLayer layer>
void QuadReference<layer>::setBackgroundColor(std::uint32_t packedColor) const
{
    logger.extraAssert(index != -1, "Set background color of invalid quad reference");

    auto& data = quadPool.getStorage<layer>().data;
    data[index].setBackgroundColor(packedColor);
}

template<QuadLayer layer>
template<bool isConstructor, IsSameType<QuadData> T>
void QuadReference<layer>::init(T&& quadData)
{
    auto& [data, references] = quadPool.getStorage<layer>();
    if constexpr (!isConstructor)
    {
        if (index != -1)
        {
            data[index] = std::forward<T>(quadData);
            return;
        }
    }

    logger.extraAssert(data.getSize() < data.getCapacity(), "Inserted quad into full quad pool");

    //Append reference to the end of the quad pool
    index = (std::int64_t)data.getSize();
    data.emplaceBack(std::forward<T>(quadData));
    references.emplaceBack(this);
}

template<QuadLayer layer>
template<bool isDestructor>
void QuadReference<layer>::cleanup()
{
    //Reference isn't valid
    if (index == -1) return;

    auto& [data, references] = quadPool.getStorage<layer>();

    //Copy last element in place of deleted and pop
    data[index] = data.getBack();
    data.popBack();

    //Update the reference index
    references[index] = references.getBack();
    references[index]->index = index;
    references.popBack();

    if constexpr (!isDestructor)
    {
        index = -1;
    }
}