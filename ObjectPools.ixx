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
	static void setData(size_t index, QuadData const& newData)
	{
		data[index] = newData;
	}
	static void insert(QuadData const& newData)
	{
		data[size] = newData;
		size++;
	}

	static auto getData() { return data.data(); }
	static auto getSize() { return size; }

private:
	inline static std::array<QuadData, 2048> data;
	inline static size_t size{};
};