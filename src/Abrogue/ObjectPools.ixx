module;

#include <glm/glm.hpp>

export module ObjectPools;

export import Constants;

export struct QuadData
{
	QuadData() = default;
	QuadData(glm::vec2 position, float scale, glm::uvec2 colors, uint32_t glyphIndex)
		:position(position), scale(tileScale * scale), colors(colors), glyphIndex(glyphIndex)
	{}

	static uint32_t packColor(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
	{
		return (uint32_t)red << 24 | (uint32_t)green << 16 | (uint32_t)blue << 8 | (uint32_t)alpha;
	}

	static constexpr glm::vec2 tileScale{0.5f / Constants::screenHeight, 1.0f / Constants::screenHeight};

	glm::vec2 position;
	glm::vec2 scale;
	glm::uvec2 colors;
	uint32_t glyphIndex;
};

export class QuadPool
{
public:
	class Reference
	{
	public:
		Reference() = default;
		Reference(size_t index): index(index) {}

		void setPosition(glm::vec2 position) const
		{
			data[index].position = position;
		}

	private:
		size_t index{};
	};

	[[nodiscard]] static Reference insert(QuadData const& newData)
	{
		data[size] = newData;
		return Reference{size++};
	}

	[[nodiscard]] static auto getData() { return data.data(); }
	[[nodiscard]] static auto getSize() { return size; }
	[[nodiscard]] static auto getCapacity() { return data.size(); }

private:
	inline static std::array<QuadData, 4096> data;
	inline static size_t size{};
};