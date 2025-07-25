module Discoveries;

using namespace std::literals;

void Discoveries::init()
{
	labels[(std::size_t)LabelType::eTitle].init("Discoveries"sv, 56, 6, QuadPool::ePopup);
}
