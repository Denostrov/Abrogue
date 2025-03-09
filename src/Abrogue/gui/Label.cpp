module Label;

import GameSystems;

void Label::setText(std::string_view text)
{
	size = text.size();

	quadReferences.reserve(size);
	for(size_t i = quadReferences.size(); i < size; i++)
	{
		quadReferences.emplace_back(quadPool.insert(QuadData{{(x + i + 0.5f) * QuadData::tileScale.x, (y + 0.5f) * QuadData::tileScale.y}, 1.0f,
													 {QuadData::packColor(255, 255, 255, 255), QuadData::packColor(255, 255, 255, 0)}, (uint32_t)text[i]}));
	}

	for(size_t i = 0; i < size; i++)
	{
		quadReferences[i].setGlyph(text[i]);
	}

	for(size_t i = size; i < quadReferences.size(); i++)
		quadReferences[i].setGlyph(' ');
}