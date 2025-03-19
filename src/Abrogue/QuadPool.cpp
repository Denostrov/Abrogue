module;

#include <glm/glm.hpp>

module QuadPool;

import GameSystems;

QuadPool::Reference::~Reference()
{
	if(index == -1) return;

	std::swap(pool->data[index], pool->data[pool->data.size() - 1]);
	pool->data.pop_back();

	if(index != pool->references.size() - 1)
		pool->references[pool->references.size() - 1]->index = index;

	std::swap(pool->references[index], pool->references[pool->references.size() - 1]);
	pool->references.pop_back();
}

QuadPool::Reference& QuadPool::Reference::operator=(QuadPool::Reference&& rhs)
{
	if(index != -1 && rhs.index != -1)
	{
		std::swap(pool->data[index], pool->data[rhs.index]);
		std::swap(pool->references[index], pool->references[rhs.index]);
	}
	else if(rhs.index != -1)
		pool->references[rhs.index] = this;

	std::swap(index, rhs.index);
	return *this;
}

void QuadPool::Reference::setPosition(glm::vec2 position) const
{
	logger.extraAssert(index != -1, "Set position of invalid quad reference");

	pool->data[index].position = position;
}

void QuadPool::Reference::setGlyph(std::uint8_t glyph) const
{
	logger.extraAssert(index != -1, "Set glyph of invalid quad reference");

	pool->data[index].glyph = glyph;
}

void QuadPool::Reference::setBackgroundColor(uint32_t packedColor) const
{
	logger.extraAssert(index != -1, "Set background color of invalid quad reference");

	pool->data[index].colors[1] = packedColor;
}

QuadPool::Reference QuadPool::insert(QuadData const& newData)
{
	logger.extraAssert(data.size() < capacity, "Inserted quad into full quad pool");

	Reference result{data.size(), this};
	data.emplace_back(newData);
	references.emplace_back(&result);

	return result;
}

void QuadPool::prepare()
{
	data.reserve(capacity);
	references.reserve(capacity);
}
