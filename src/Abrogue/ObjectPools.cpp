module;

#include <glm/glm.hpp>

module ObjectPools;

QuadPool::Reference::~Reference()
{
	if(index == -1) return;

	std::swap(data[index], data[data.size() - 1]);
	data.pop_back();

	std::swap(references[index], references[references.size() - 1]);
	references.pop_back();
}

QuadPool::Reference& QuadPool::Reference::operator=(QuadPool::Reference&& rhs)
{
	if(index != -1 && rhs.index != -1)
		std::swap(QuadPool::references[index], QuadPool::references[rhs.index]);
	else if(rhs.index != -1)
		QuadPool::references[rhs.index] = this;

	std::swap(index, rhs.index);
	return *this;
}

void QuadPool::Reference::setPosition(glm::vec2 position) const
{
	assert(index != -1);

	data[index].position = position;
}

QuadPool::Reference QuadPool::insert(QuadData const& newData)
{
	assert(data.size() < capacity);

	Reference result{data.size()};
	data.emplace_back(newData);
	references.emplace_back(&result);

	return result;
}

void QuadPool::init()
{
	data.reserve(8192);
	references.reserve(8192);
}

void QuadPool::release()
{
	for(auto reference : references) reference->index = -1;

	data.clear();
	references.clear();
}
