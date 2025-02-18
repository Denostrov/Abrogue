module;

#include <glm/glm.hpp>

export module ObjectPools;

export import std;

export struct QuadData
{
	glm::vec2 pos;
	glm::vec2 scale;
};

export class QuadPool
{
public:
	class Reference
	{
	public:
		Reference() = default;
		Reference(size_t index): index(index) {}

		void set(QuadData const& newData) const
		{
			data[index] = newData;
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

private:
	inline static std::array<QuadData, 2048> data;
	inline static size_t size{};
};