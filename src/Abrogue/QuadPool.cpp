module;

#include <glm/glm.hpp>

module QuadPool;

import Logger;

QuadPool::Reference::~Reference()
{
	//Reference isn't valid
	if(index == -1) return;

	auto& data = quadPool.data[layer];
	auto& references = quadPool.references[layer];

	//Copy last element in place of deleted and pop
	data[index] = data.back();
	data.pop_back();

	//Update the reference index
	references[index] = references.back();
	references[index]->index = index;
	references.pop_back();

	quadPool.size--;
}

QuadPool::Reference& QuadPool::Reference::operator=(QuadPool::Reference&& rhs)
{
	//Update reference pointers in quad pool
	if(index != -1)
	{
		if(rhs.index != -1)
			std::swap(quadPool.references[layer][index], quadPool.references[rhs.layer][rhs.index]);
		else
			quadPool.references[layer][index] = &rhs;
	}
	else if(rhs.index != -1)
		quadPool.references[rhs.layer][rhs.index] = this;

	std::swap(index, rhs.index);
	std::swap(layer, rhs.layer);
	return *this;
}

void QuadPool::Reference::setPosition(float positionX, float positionY) const
{
	logger.extraAssert(index != -1, "Set position of invalid quad reference");

	quadPool.data[layer][index].setPosition(positionX, positionY);
}

void QuadPool::Reference::setGlyph(std::uint8_t glyph) const
{
	logger.extraAssert(index != -1, "Set glyph of invalid quad reference");

	quadPool.data[layer][index].setGlyph(glyph);
}

void QuadPool::Reference::setColor(uint32_t packedColor) const
{
	logger.extraAssert(index != -1, "Set color of invalid quad reference");

	quadPool.data[layer][index].setColor(packedColor);
}

void QuadPool::Reference::setBackgroundColor(uint32_t packedColor) const
{
	logger.extraAssert(index != -1, "Set background color of invalid quad reference");

	quadPool.data[layer][index].setBackgroundColor(packedColor);
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

QuadPool::~QuadPool()
{
	for(auto const& referencePool : references)
	{
		for(auto reference : referencePool)
			reference->index = -1;
	}
}

void QuadPool::prepare()
{
	for(size_t i = 0; i < COUNT; i++)
	{
		data[i].reserve(capacities[i]);
		references[i].reserve(capacities[i]);
	}
}
