export module Label;

export import ObjectPools;

export class Label
{
public:
	Label() = default;
	Label(std::string_view text, std::uint32_t x, std::uint32_t y)
		:x(x), y(y)
	{
		quadReferences.reserve(text.size());
		for(auto c : text)
		{
			quadReferences.emplace_back(QuadPool::insert(QuadData{{0.0f, 0.0f}, 1.0f,
														 {QuadData::packColor(255, 255, 255, 255), QuadData::packColor(255, 255, 255, 0)}, (uint32_t)c}));
		}

		for(size_t i = 0; i < text.size(); i++)
		{
			quadReferences[i].setPosition({(x + i + 0.5f) * QuadData::tileScale.x, (y + 0.5f) * QuadData::tileScale.y});
		}
	}

	void setText(std::string_view text)
	{
		quadReferences.reserve(text.size());
		for(size_t i = quadReferences.size(); i < text.size(); i++)
		{
			quadReferences.emplace_back(QuadPool::insert(QuadData{{(x + i + 0.5f) * QuadData::tileScale.x, (y + 0.5f) * QuadData::tileScale.y}, 1.0f,
														 {QuadData::packColor(255, 255, 255, 255), QuadData::packColor(255, 255, 255, 0)}, (uint32_t)text[i]}));
		}

		for(size_t i = 0; i < text.size(); i++)
		{
			quadReferences[i].setGlyph(text[i]);
		}

		for(size_t i = text.size(); i < quadReferences.size(); i++)
			quadReferences[i].setGlyph(' ');
	}

	void clear()
	{
		for(size_t i = 0; i < quadReferences.size(); i++)
			quadReferences[i].setGlyph(' ');
	}

private:
	std::uint32_t x{}, y{};
	std::vector<QuadPool::Reference> quadReferences;
};