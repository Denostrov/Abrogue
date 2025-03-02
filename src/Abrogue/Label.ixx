export module Label;

export import ObjectPools;

export class Label
{
public:
	Label() = default;
	Label(std::string_view text, std::uint32_t x, std::uint32_t y)
		:text(text)
	{
		for(auto c : text)
		{
			quadReferences.emplace_back(QuadPool::insert(QuadData{{0.0f, 0.0f}, 1.0f,
														 {QuadData::packColor(255, 255, 255, 255), QuadData::packColor(255, 255, 255, 0)}, (uint32_t)c}));
		}

		auto startX = x;
		for(size_t i = 0; i < text.size(); i++)
		{
			quadReferences[i].setPosition({(x + 0.5f) * QuadData::tileScale.x, (y + 0.5f) * QuadData::tileScale.y});
			x++;
		}
	}

private:
	std::string text;
	std::vector<QuadPool::Reference> quadReferences;
};