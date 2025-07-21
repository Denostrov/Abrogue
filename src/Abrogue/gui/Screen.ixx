export module Screen;

export import Label;

//Base class for GUI screens
export class Screen
{
public:
	Screen() = default;

	void updateMouseMoved(std::int64_t x, std::int64_t y);
	template<class Self>
	void updateMousePressed(this Self&& self, std::int64_t x, std::int64_t y)
	{
		for(size_t i = 0; i < self.pressableButtons.size(); i++)
		{
			if(!self.pressableButtons[i].checkCollision(x, y))
				continue;

			self.onButtonPressed(i);
			return;
		}
	}

protected:
	std::span<Label> pressableButtons;

private:
	Label* hoveredButton{};
};