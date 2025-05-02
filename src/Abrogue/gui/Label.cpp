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
											 {Helpers::packColor(255, 255, 255, 255), getBackgroundColor(i)}, (uint32_t)text[i]}, layer));
	}
}

void Label::setHovered(bool hovered)
{
	if(isHovered == hovered)
		return;

	isHovered = hovered;

	for(size_t i = 0; i < size; i++)
		quadReferences[i].setBackgroundColor(getBackgroundColor(i));
}

void Label::setPressed(bool pressed)
{
	if(isPressed == pressed)
		return;

	isPressed = pressed;

	for(size_t i = 0; i < size; i++)
		quadReferences[i].setBackgroundColor(getBackgroundColor(i));
}

void Label::setText(std::string_view newText)
{
	text = newText;
	if(!isVisible)
		return;

	size = text.size();

	//Create remaining quads when new text is longer
	quadReferences.reserve(size);
	for(size_t i = quadReferences.size(); i < size; i++)
	{
		quadReferences.emplace_back(quadPool.insert(QuadData{{(x + i + 0.5f) * QuadData::tileScale.x, (y + 0.5f) * QuadData::tileScale.y},
													 {Helpers::packColor(255, 255, 255, 255), getBackgroundColor(i)}, (uint32_t)text[i]}, layer));
	}

	//Set existing quad parameters
	for(size_t i = 0; i < size; i++)
	{
		quadReferences[i].setGlyph(text[i]);
		quadReferences[i].setBackgroundColor(getBackgroundColor(i));
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

	for(size_t i = 0; i < size; i++)
		quadReferences[i].setBackgroundColor(getBackgroundColor(i));
}

void Label::setPressedBackgroundColor(std::uint32_t color, std::uint32_t hoverColor)
{
	pressedBackgroundColor = color;
	hoveredPressedBackgroundColor = hoverColor;

	for(size_t i = 0; i < size; i++)
		quadReferences[i].setBackgroundColor(getBackgroundColor(i));
}

void Label::setProgress(double percentage)
{
	progress = percentage;

	for(size_t i = 0; i < size; i++)
		quadReferences[i].setBackgroundColor(getBackgroundColor(i));
}

std::uint32_t Label::getBackgroundColor(std::size_t index) const
{
	auto [r, g, b, a] = Helpers::unpackColor(isPressed ? (isHovered ? hoveredPressedBackgroundColor : pressedBackgroundColor)
											 : (isHovered ? hoveredBackgroundColor : backgroundColor));
	double colorCoefficient = std::clamp((progress - (double)index / size) * size, 0.0, 1.0);
	return Helpers::packColor(r * colorCoefficient, g * colorCoefficient, b * colorCoefficient, a);
}
