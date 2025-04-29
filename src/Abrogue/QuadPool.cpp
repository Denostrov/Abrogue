module;

#include <glm/glm.hpp>

module QuadPool;

import GameSystems;

QuadPool::Reference::~Reference()
{
	if(index == -1) return;

	auto& data = quadPool.data[layer];
	auto& references = quadPool.references[layer];

	std::swap(data[index], data[data.size() - 1]);
	data.pop_back();

	if(index != references.size() - 1)
		references[references.size() - 1]->index = index;

	std::swap(references[index], references[references.size() - 1]);
	references.pop_back();

	quadPool.size--;
}

QuadPool::Reference& QuadPool::Reference::operator=(QuadPool::Reference&& rhs)
{
	if(index != -1 && rhs.index != -1)
	{
		auto& references = quadPool.references[layer];
		std::swap(references[index], references[rhs.index]);
	}
	else if(rhs.index != -1)
	{
		auto& references = quadPool.references[rhs.layer];
		references[rhs.index] = this;
	}

	std::swap(index, rhs.index);
	std::swap(layer, rhs.layer);
	return *this;
}

void QuadPool::Reference::setPosition(glm::vec2 position) const
{
	logger.extraAssert(index != -1, "Set position of invalid quad reference");

	quadPool.data[layer][index].position = position;
}

void QuadPool::Reference::setGlyph(std::uint8_t glyph) const
{
	logger.extraAssert(index != -1, "Set glyph of invalid quad reference");

	quadPool.data[layer][index].glyph = glyph;
}

void QuadPool::Reference::setBackgroundColor(uint32_t packedColor) const
{
	logger.extraAssert(index != -1, "Set background color of invalid quad reference");

	quadPool.data[layer][index].colors[1] = packedColor;
}

QuadPool::Reference QuadPool::insert(QuadData const& newData, Layer layer)
{
	logger.extraAssert(data[layer].size() < data[layer].capacity(), "Inserted quad into full quad pool");

	Reference result{data[layer].size(), layer};
	data[layer].emplace_back(newData);
	references[layer].emplace_back(&result);

	size++;

	return result;
}

void QuadPool::prepare()
{
	for(size_t i = 0; i < COUNT; i++)
	{
		data[i].reserve(capacities[i]);
		references[i].reserve(capacities[i]);
	}
}
