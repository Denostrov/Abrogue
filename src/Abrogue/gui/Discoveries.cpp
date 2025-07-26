module Discoveries;

using namespace std::literals;

void Discoveries::init()
{
	labels[LabelType::eTitle].init("Discoveries"sv, 56, 6, QuadPool::ePopup);
}
