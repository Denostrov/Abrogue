export module Button;

export import Label;

export class Button
{
public:
	Button() = default;
	Button(std::string_view text, std::uint32_t x, std::uint32_t y)
		:label(text, x, y)
	{}

	void setText(std::string_view text)
	{
		label.setText(text);
	}

	void setPosition(std::uint32_t x, std::uint32_t y)
	{
		label.setPosition(x, y);
	}

	void clear()
	{
		label.clear();
	}

	bool checkCollision(std::uint32_t x, std::uint32_t y)
	{
		auto [labelX, labelY] = label.getPosition();
		auto size = label.getSize();
		if(labelX <= x && x < labelX + size && labelY <= y && y < labelY + 1)
			return true;

		return false;
	}

private:
	Label label;
};