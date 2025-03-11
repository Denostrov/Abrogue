module;

#include <glm/glm.hpp>

export module QuadPool;

export import Constants;

export struct QuadData
{
	QuadData() = default;
	QuadData(glm::vec2 position, float scale, glm::uvec2 colors, uint32_t glyph)
		:position(position), scale(tileScale* scale), colors(colors), glyph(glyph)
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
	class Reference
	{
	public:
		Reference() = default;
		Reference(size_t index): index(index) {}
		~Reference();

		Reference(Reference&& rhs) { *this = std::move(rhs); }
		Reference& operator=(Reference&& rhs);

		void setPosition(glm::vec2 position) const;
		void setGlyph(std::uint8_t glyph) const;
		void setBackgroundColor(uint32_t packedColor) const;

	private:
		int64_t index{-1};

		friend class QuadPool;
	};

	[[nodiscard]] Reference insert(QuadData const& newData);

	[[nodiscard]] auto const getData() { return data.data(); }
	[[nodiscard]] auto getSize() { return data.size(); }
	[[nodiscard]] auto getCapacity() { return capacity; }

private:
	void prepare();

	static constexpr size_t capacity{8192};

	std::vector<QuadData> data;
	std::vector<Reference*> references;

	friend class Game;
};


