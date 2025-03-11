module Label;

import GameSystems;

void Label::init(std::string_view text, std::uint32_t x, std::uint32_t y, bool visible)
{
	setPosition(x, y);
	setText(text);

	setVisible(visible);
}

void Label::setVisible(bool visible)
{
	isVisible = visible;
	size = visible ? text.size() : 0;
	for(size_t i = 0; i < quadReferences.size(); i++)
	{
		quadReferences[i].setGlyph(visible ? text[i] : ' ');
		quadReferences[i].setBackgroundColor(visible ? backgroundColor : 0);
	}
}

void Label::setHovered(bool hovered)
{
	isHovered = hovered;
	for(size_t i = 0; i < size; i++)
	{
		quadReferences[i].setBackgroundColor(hovered ? hoverBackgroundColor : backgroundColor);
	}
}

void Label::setText(std::string_view newText)
{
	text = newText;
	if(isVisible)
		size = text.size();

	quadReferences.reserve(text.size());
	for(size_t i = quadReferences.size(); i < text.size(); i++)
	{
		quadReferences.emplace_back(quadPool.insert(QuadData{{(x + i + 0.5f) * QuadData::tileScale.x, (y + 0.5f) * QuadData::tileScale.y}, 1.0f,
													 {Helpers::packColor(255, 255, 255, 255), isVisible ? backgroundColor : 0}, isVisible ? (uint32_t)text[i] : ' '}));
	}

	for(size_t i = 0; i < size; i++)
	{
		quadReferences[i].setGlyph(text[i]);
	}

	for(size_t i = size; i < quadReferences.size(); i++)
		quadReferences[i].setGlyph(' ');
}

void Label::setPosition(std::uint32_t newX, std::uint32_t newY)
{
	x = newX;
	y = newY;
	for(size_t i = 0; i < quadReferences.size(); i++)
	{
		quadReferences[i].setPosition({(x + i + 0.5f) * QuadData::tileScale.x, (y + 0.5f) * QuadData::tileScale.y});
	}
}

void Label::setBackgroundColor(std::uint32_t color, std::uint32_t hoverColor)
{
	backgroundColor = color;
	hoverBackgroundColor = hoverColor;
	for(size_t i = 0; i < size; i++)
	{
		quadReferences[i].setBackgroundColor(isHovered ? hoverBackgroundColor : backgroundColor);
	}
}
