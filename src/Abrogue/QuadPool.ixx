module;

#include <glm/glm.hpp>

export module QuadPool;

export import Constants;

export struct QuadData
{
	QuadData() = default;
	QuadData(glm::vec2 position, float scale, glm::uvec2 colors, uint32_t glyph)
		:position(position), scale(tileScale * scale), colors(colors), glyph(glyph)
	{}

	static constexpr glm::vec2 tileScale{0.5f / Constants::screenHeight, 1.0f / Constants::screenHeight};

	glm::vec2 position;
	glm::vec2 scale;
	glm::uvec2 colors;
	uint32_t glyph;
};

export class QuadPool
{
public:
	enum Layer
	{
		eMap,
		eItem,
		eEntity,
		ePopupBackground,
		ePopup,
		COUNT
	};

	class Reference
	{
	public:
		Reference() = default;
		~Reference();

		Reference(Reference&& rhs) { *this = std::move(rhs); }
		Reference& operator=(Reference&& rhs);

		void setPosition(glm::vec2 position) const;
		void setGlyph(std::uint8_t glyph) const;
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


