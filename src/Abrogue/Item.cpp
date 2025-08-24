module Item;

import Map;

using namespace std::literals;

Item::Item(Type newType, double x, double y)
	:PhysicsComponent(x, y, 0.48, 0.48, 0.48, 0.48)
{
	type = newType;

	quad = quadPool.insert(QuadData{{Constants::mapOffset + x, y}, {Color::pack(255, 255, 0, 255), Color::pack(255, 255, 0, 0)}, typeGlyphs[(size_t)newType]}, QuadPool::eItem);
}

void Item::update()
{
	PhysicsComponent::update();
}

void Item::updateDraw(double deltaTime)
{
	auto [x, y] = getPosition();
	auto [vx, vy] = getVelocity();
	quad.setPosition(Constants::mapOffset + x + vx * deltaTime, y + vy * deltaTime);

	auto brightness = map.getTileBrightness(x, y);
	if(brightness < Constants::mapMinBrightness)
	{
		quad.setColor(0);
		quad.setBackgroundColor(0);
	}
	else
	{
		Color itemColor(255, 255, 0, 255);
		itemColor.multiplyRGB(brightness);
		quad.setColor(itemColor.getPacked());
		quad.setBackgroundColor(itemColor.getTransparentPacked());
	}
}

FixedString<32> Item::getName() const
{
	FixedString<32> result;
	if(type == Type::eFood)
		result.fill("Food"sv);
	else if(type == Type::eAmulet)
		result.fill("Amulet of Yendor"sv);

	return result;
}

void Item::setVisible(bool visible)
{
	if(visible)
	{
		auto [x, y] = getPosition();
		quad = quadPool.insert(QuadData{{Constants::mapOffset + x, y}, {Color::pack(255, 255, 0, 255), Color::pack(255, 255, 0, 0)}, typeGlyphs[(size_t)type]}, QuadPool::eItem);
	}
	else
		quad = QuadPool::Reference{};
}
