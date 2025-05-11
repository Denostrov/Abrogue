export module Label;

export import QuadPool;
export import FixedVector;

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

	[[nodiscard]] auto getPressed() const { return isPressed; }

	void setVisible(bool visible);
	void setHovered(bool hovered);
	void setPressed(bool pressed);

	void setText(std::string_view text);
	void setPosition(std::int64_t newX, std::int64_t newY);
	void setBackgroundColor(PackedColor color, PackedColor hoverColor);
	void setPressedBackgroundColor(PackedColor color, PackedColor hoverColor);
	void setProgress(double percentage);

private:
	PackedColor getBackgroundColor(std::int64_t index) const;

	bool isVisible{};
	bool isHovered{};
	bool isPressed{};

	std::string text;
	QuadPool::Layer layer{};

	std::int64_t x{}, y{};
	std::size_t size{};
	double progress{1.0};

	PackedColor backgroundColor{Constants::labelBackgroundColor};
	PackedColor hoveredBackgroundColor{Constants::labelHoveredColor};
	PackedColor pressedBackgroundColor{Constants::labelPressedColor};
	PackedColor hoveredPressedBackgroundColor{Constants::labelHoveredPressedColor};

	FixedVector<QuadPool::Reference, 128> quadReferences;
};