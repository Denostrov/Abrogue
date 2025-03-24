export module Label;

export import QuadPool;

//Class for handling clickable text boxes
export class Label
{
public:
	Label() = default;

	void init(std::string_view text, std::uint32_t x, std::uint32_t y, QuadPool::Layer drawLayer, bool visible = false);

	[[nodiscard]] bool checkCollision(std::uint32_t checkX, std::uint32_t checkY) const
	{
		return x <= checkX && checkX < x + size && y <= checkY && checkY < y + 1;
	}

	void togglePressed() { setPressed(!isPressed); }

	auto getPressed() const { return isPressed; }

	void setVisible(bool visible);
	void setHovered(bool hovered);
	void setPressed(bool pressed);

	void setText(std::string_view text);
	void setPosition(std::uint32_t newX, std::uint32_t newY);
	void setBackgroundColor(std::uint32_t color, std::uint32_t hoverColor);
	void setPressedBackgroundColor(std::uint32_t color, std::uint32_t hoverColor);

private:
	std::uint32_t getBackgroundColor() const;

	bool isVisible{};
	bool isHovered{};
	bool isPressed{};

	std::string text;
	QuadPool::Layer layer{};

	std::uint32_t x{}, y{};
	std::size_t size{};

	std::uint32_t backgroundColor{Constants::labelBackgroundColor};
	std::uint32_t hoveredBackgroundColor{Constants::labelHoveredColor};
	std::uint32_t pressedBackgroundColor{Constants::labelPressedColor};
	std::uint32_t hoveredPressedBackgroundColor{Constants::labelHoveredPressedColor};

	std::vector<QuadPool::Reference> quadReferences;
};