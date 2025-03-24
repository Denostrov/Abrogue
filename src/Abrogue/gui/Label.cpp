module Label;

import GameSystems;

void Label::init(std::string_view text, std::uint32_t x, std::uint32_t y, QuadPool::Layer drawLayer, bool visible)
{
	layer = drawLayer;
	setPosition(x, y);
	setText(text);

	setVisible(visible);
}

void Label::setVisible(bool visible)
{
	if(isVisible == visible)
		return;

	//Invisible label has size 0 to disable collision detection
	isVisible = visible;
	size = visible ? text.size() : 0;

	//Delete invisible quads to avoid overdraw
	if(!visible)
	{
		quadReferences.clear();
		return;
	}

	//Recreate quads
	for(size_t i = 0; i < size; i++)
	{
		quadReferences.emplace_back(quadPool.insert(QuadData{{(x + i + 0.5f) * QuadData::tileScale.x, (y + 0.5f) * QuadData::tileScale.y},
											 {Helpers::packColor(255, 255, 255, 255), backgroundColor}, (uint32_t)text[i]}, layer));
	}
}

void Label::setHovered(bool hovered)
{
	if(isHovered == hovered)
		return;

	isHovered = hovered;

	auto color = getBackgroundColor();
	for(size_t i = 0; i < size; i++)
		quadReferences[i].setBackgroundColor(color);
}

void Label::setPressed(bool pressed)
{
	if(isPressed == pressed)
		return;

	isPressed = pressed;

	auto color = getBackgroundColor();
	for(size_t i = 0; i < size; i++)
		quadReferences[i].setBackgroundColor(color);
}

void Label::setText(std::string_view newText)
{
	text = newText;
	if(!isVisible)
		return;

	size = text.size();

	//Create remaining quads when new text is longer
	auto color = getBackgroundColor();
	quadReferences.reserve(size);
	for(size_t i = quadReferences.size(); i < size; i++)
	{
		quadReferences.emplace_back(quadPool.insert(QuadData{{(x + i + 0.5f) * QuadData::tileScale.x, (y + 0.5f) * QuadData::tileScale.y},
													 {Helpers::packColor(255, 255, 255, 255), color}, (uint32_t)text[i]}, layer));
	}

	//Set existing quad parameters
	for(size_t i = 0; i < size; i++)
	{
		quadReferences[i].setGlyph(text[i]);
		quadReferences[i].setBackgroundColor(color);
	}

	//Erase extra quads when new text is shorter
	quadReferences.erase(quadReferences.begin() + size, quadReferences.end());
}

void Label::setPosition(std::uint32_t newX, std::uint32_t newY)
{
	x = newX;
	y = newY;
	for(size_t i = 0; i < size; i++)
		quadReferences[i].setPosition({(x + i + 0.5f) * QuadData::tileScale.x, (y + 0.5f) * QuadData::tileScale.y});
}

void Label::setBackgroundColor(std::uint32_t color, std::uint32_t hoverColor)
{
	backgroundColor = color;
	hoveredBackgroundColor = hoverColor;

	auto newColor = getBackgroundColor();
	for(size_t i = 0; i < size; i++)
		quadReferences[i].setBackgroundColor(newColor);
}

void Label::setPressedBackgroundColor(std::uint32_t color, std::uint32_t hoverColor)
{
	pressedBackgroundColor = color;
	hoveredPressedBackgroundColor = hoverColor;

	auto newColor = getBackgroundColor();
	for(size_t i = 0; i < size; i++)
		quadReferences[i].setBackgroundColor(newColor);
}

std::uint32_t Label::getBackgroundColor() const
{
	return isPressed ? (isHovered ? hoveredPressedBackgroundColor : pressedBackgroundColor)
		: (isHovered ? hoveredBackgroundColor : backgroundColor);
}
