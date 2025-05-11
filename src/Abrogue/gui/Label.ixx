export module Label;

export import QuadPool;

//Class for handling clickable text boxes
export class Label
{
public:
	Label() = default;

	void init(std::string_view text, std::int64_t x, std::int64_t y, QuadPool::Layer drawLayer, bool visible = false);

	[[nodiscard]] bool checkCollision(std::int64_t checkX, std::int64_t checkY) const
	{
		return x <= checkX && checkX < x + size && y <= checkY && checkY < y + 1;
	}

	void togglePressed() { setPressed(!isPressed); }

	auto getPressed() const { return isPressed; }

	void setVisible(bool visible);
	void setHovered(bool hovered);
	void setPressed(bool pressed);

	void setText(std::string_view text);
	void setPosition(std::int64_t newX, std::int64_t newY);
	void setBackgroundColor(std::uint32_t color, std::uint32_t hoverColor);
	void setPressedBackgroundColor(std::uint32_t color, std::uint32_t hoverColor);
	void setProgress(double percentage);

private:
	std::uint32_t getBackgroundColor(std::size_t index) const;

	bool isVisible{};
	bool isHovered{};
	bool isPressed{};

	std::string text;
	QuadPool::Layer layer{};

	std::int64_t x{}, y{};
	std::size_t size{};
	double progress{1.0};

	std::uint32_t backgroundColor{Constants::labelBackgroundColor};
	std::uint32_t hoveredBackgroundColor{Constants::labelHoveredColor};
	std::uint32_t pressedBackgroundColor{Constants::labelPressedColor};
	std::uint32_t hoveredPressedBackgroundColor{Constants::labelHoveredPressedColor};

	std::vector<QuadPool::Reference> quadReferences;
};