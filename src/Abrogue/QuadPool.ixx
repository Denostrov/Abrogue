module;

#include <glm/glm.hpp>

export module QuadPool;

export import Constants;

export class QuadData
{
public:
	QuadData() = default;
	QuadData(glm::vec2 position, glm::uvec2 colors, uint32_t glyph)
		:position(position.x * Constants::tileScaleX, position.y * Constants::tileScaleY), scale(Constants::tileScaleX, Constants::tileScaleY),
		rotation(1.0, 0.0), colors(colors), glyph(glyph)
	{}

	void setPosition(float positionX, float positionY) { position = {positionX * Constants::tileScaleX, positionY * Constants::tileScaleY}; }
	void setScale(float scaleX, float scaleY) { scale = {Constants::tileScaleX * scaleX, Constants::tileScaleY * scaleY}; }
	void setRotation(float angle) { rotation = {std::cos(angle), std::sin(angle)}; }
	void setRotation(float cos, float sin) { rotation = {cos, sin}; }
	void setColor(std::uint32_t packedColor) { colors[0] = packedColor; }
	void setBackgroundColor(std::uint32_t packedColor) { colors[1] = packedColor; }
	void setGlyph(std::uint32_t newGlyph) { glyph = newGlyph; }

private:
	glm::vec2 position{};
	glm::vec2 scale{};
	glm::vec2 rotation{};
	glm::uvec2 colors{};
	uint32_t glyph{};
};

//Object pool for unordered quad data storage
export class QuadPool
{
public:
	//Layers for defining draw order
	enum Layer
	{
		eMap,
		eItem,
		eEntity,
		ePopupBackground,
		ePopup,
		COUNT
	};

	//Bidirectional reference for modifying quad data
	class Reference
	{
	public:
		Reference() = default;
		~Reference();

		Reference(Reference&& rhs) { *this = std::move(rhs); }
		Reference& operator=(Reference&& rhs);

		void setPosition(float positionX, float positionY) const;
		void setGlyph(std::uint8_t glyph) const;
		void setColor(uint32_t packedColor) const;
		void setBackgroundColor(uint32_t packedColor) const;

	private:
		Reference(size_t index, Layer layer): index(index), layer(layer) {}

		int64_t index{-1};
		Layer layer{COUNT};

		friend class QuadPool;
	};

	void prepare();

	[[nodiscard]] Reference insert(QuadData const& newData, Layer layer);

	[[nodiscard]] auto const& getData() const { return data; }
	[[nodiscard]] auto getSize() const { return size; }
	[[nodiscard]] constexpr auto getCapacity() const { return std::accumulate(capacities.begin(), capacities.end(), 0); }

private:
	static constexpr std::array<size_t, COUNT> capacities{8192, 8192, 8192, 512, 2048};

	std::array<std::vector<QuadData>, COUNT> data;
	std::array<std::vector<Reference*>, COUNT> references;
	size_t size{};
};


