module;

#include <glm/glm.hpp>

module QuadPool;

import GameSystems;

QuadPool::Reference::~Reference()
{
	if(index == -1) return;

	std::swap(quadPool.data[index], quadPool.data[quadPool.data.size() - 1]);
	quadPool.data.pop_back();

	if(index != quadPool.references.size() - 1)
		quadPool.references[quadPool.references.size() - 1]->index = index;

	std::swap(quadPool.references[index], quadPool.references[quadPool.references.size() - 1]);
	quadPool.references.pop_back();
}

QuadPool::Reference& QuadPool::Reference::operator=(QuadPool::Reference&& rhs)
{
	if(index != -1 && rhs.index != -1)
	{
		std::swap(quadPool.data[index], quadPool.data[rhs.index]);
		std::swap(quadPool.references[index], quadPool.references[rhs.index]);
	}
	else if(rhs.index != -1)
		quadPool.references[rhs.index] = this;

	std::swap(index, rhs.index);
	return *this;
}

void QuadPool::Reference::setPosition(glm::vec2 position) const
{
	logger.extraAssert(index != -1, "Set position of invalid quad reference");

	quadPool.data[index].position = position;
}

void QuadPool::Reference::setGlyph(std::uint8_t glyph) const
{
	logger.extraAssert(index != -1, "Set glyph of invalid quad reference");

	quadPool.data[index].glyph = glyph;
}

void QuadPool::Reference::setBackgroundColor(uint32_t packedColor) const
{
	logger.extraAssert(index != -1, "Set background color of invalid quad reference");

	quadPool.data[index].colors[1] = packedColor;
}

QuadPool::Reference QuadPool::insert(QuadData const& newData)
{
	logger.extraAssert(data.size() < capacity, "Inserted quad into full quad pool");

	Reference result{data.size()};
	data.emplace_back(newData);
	references.emplace_back(&result);

	return result;
}

void QuadPool::prepare()
{
	data.reserve(8192);
	references.reserve(8192);
}
