export module Label;

export import QuadPool;

//Class for handling clickable text boxes
export class Label
{
public:
	Label() = default;

	void init(std::string_view text, std::uint32_t x, std::uint32_t y, bool visible = false);

	[[nodiscard]] bool checkCollision(std::uint32_t checkX, std::uint32_t checkY) const
	{
		return x <= checkX && checkX < x + size && y <= checkY && checkY < y + 1;
	}

	void setVisible(bool visible);
	void setHovered(bool hovered);

	void setText(std::string_view text);
	void setPosition(std::uint32_t newX, std::uint32_t newY);
	void setBackgroundColor(std::uint32_t color, std::uint32_t hoverColor);

private:
	bool isVisible{};
	bool isHovered{};

	std::string text;

	std::uint32_t x{}, y{};
	std::size_t size{};

	std::uint32_t backgroundColor{Constants::labelBackgroundColor};
	std::uint32_t hoverBackgroundColor{Constants::labelHoverColor};
	std::vector<QuadPool::Reference> quadReferences;
};