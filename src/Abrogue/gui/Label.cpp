module Label;

import GameSystems;

void Label::init(std::string_view text, std::int64_t x, std::int64_t y, QuadPool::Layer drawLayer, bool visible)
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
		quadReferences.emplace_back(quadPool.insert(QuadData{{x + i + 0.5f, y + 0.5f},
											 {Color::pack(255, 255, 255, 255), getBackgroundColor(i)}, (uint32_t)text[i]}, layer));
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
	for(size_t i = quadReferences.size(); i < size; i++)
	{
		quadReferences.emplace_back(quadPool.insert(QuadData{{x + i + 0.5f, y + 0.5f},
													 {Color::pack(255, 255, 255, 255), getBackgroundColor(i)}, (uint32_t)text[i]}, layer));
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

void Label::setPosition(std::int64_t newX, std::int64_t newY)
{
	x = newX;
	y = newY;
	for(size_t i = 0; i < size; i++)
		quadReferences[i].setPosition(x + i + 0.5f, y + 0.5f);
}

void Label::setBackgroundColor(PackedColor color, PackedColor hoverColor)
{
	backgroundColor = color;
	hoveredBackgroundColor = hoverColor;

	for(size_t i = 0; i < size; i++)
		quadReferences[i].setBackgroundColor(getBackgroundColor(i));
}

void Label::setPressedBackgroundColor(PackedColor color, PackedColor hoverColor)
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

PackedColor Label::getBackgroundColor(std::int64_t index) const
{
	Color color = isPressed ? (isHovered ? hoveredPressedBackgroundColor : pressedBackgroundColor)
		: (isHovered ? hoveredBackgroundColor : backgroundColor);

	//Adjust colors to form a gradient from left to right depending on progress
	double colorCoefficient = std::clamp((progress - (double)index / size) * size, 0.0, 1.0);
	color.r *= colorCoefficient;
	color.g *= colorCoefficient;
	color.b *= colorCoefficient;
	return color.getPacked();
}
