export module Label;

export import ObjectPools;

export class Label
{
public:
	Label() = default;
	Label(std::string_view text, std::uint32_t x, std::uint32_t y)
		:x(x), y(y)
	{
		setText(text);
		setPosition(x, y);
	}

	void setText(std::string_view text)
	{
		size = text.size();

		quadReferences.reserve(size);
		for(size_t i = quadReferences.size(); i < size; i++)
		{
			quadReferences.emplace_back(QuadPool::insert(QuadData{{(x + i + 0.5f) * QuadData::tileScale.x, (y + 0.5f) * QuadData::tileScale.y}, 1.0f,
														 {QuadData::packColor(255, 255, 255, 255), QuadData::packColor(255, 255, 255, 0)}, (uint32_t)text[i]}));
		}

		for(size_t i = 0; i < size; i++)
		{
			quadReferences[i].setGlyph(text[i]);
		}

		for(size_t i = size; i < quadReferences.size(); i++)
			quadReferences[i].setGlyph(' ');
	}

	void clear()
	{
		for(size_t i = 0; i < size; i++)
			quadReferences[i].setGlyph(' ');
	}

	void setPosition(std::uint32_t newX, std::uint32_t newY)
	{
		x = newX;
		y = newY;
		for(size_t i = 0; i < quadReferences.size(); i++)
		{
			quadReferences[i].setPosition({(x + i + 0.5f) * QuadData::tileScale.x, (y + 0.5f) * QuadData::tileScale.y});
		}
	}

	auto getPosition() const { return std::pair{x, y}; }
	auto getSize() const { return size; }

private:
	std::uint32_t x{}, y{};
	std::size_t size{};
	std::vector<QuadPool::Reference> quadReferences;
};