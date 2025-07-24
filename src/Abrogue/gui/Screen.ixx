export module Screen;

export import Label;

enum class EmptyButtonType
{
	COUNT
};

template<class T>
concept HasButtons = (std::size_t)T::COUNT > 0;

//Base class for GUI screens
export template<class Derived, class DerivedButtonType = EmptyButtonType, class DerivedTabButtonType = EmptyButtonType>
class ScreenComponent
{
public:
	using TabButtonType = DerivedTabButtonType;

	ScreenComponent() = default;

	void updateMouseMoved(std::int64_t x, std::int64_t y)
	{
		if constexpr(HasButtons<DerivedButtonType>)
		{
			for(std::size_t i = 0; i < buttons.size(); i++)
			{
				if(!buttons[i].checkCollision(x, y))
					continue;

				auto button = &buttons[i];
				if(button == hoveredButton)
					return;

				if(hoveredButton)
					hoveredButton->setHovered(false);

				button->setHovered(true);
				hoveredButton = button;
				return;
			}
		}

		if constexpr(HasButtons<DerivedTabButtonType>)
		{
			for(std::size_t i = 0; i < tabButtons.size(); i++)
			{
				if(!tabButtons[i].checkCollision(x, y))
					continue;

				auto button = &tabButtons[i];
				if(button == hoveredButton)
					return;

				if(hoveredButton)
					hoveredButton->setHovered(false);

				button->setHovered(true);
				hoveredButton = button;
				return;
			}
		}

		if(!hoveredButton)
			return;

		hoveredButton->setHovered(false);
		hoveredButton = nullptr;
	}
	void updateMousePressed(std::int64_t x, std::int64_t y)
	{
		if constexpr(HasButtons<DerivedButtonType>)
		{
			for(size_t i = 0; i < buttons.size(); i++)
			{
				if(!buttons[i].checkCollision(x, y))
					continue;

				static_cast<Derived*>(this)->onButtonPressed((DerivedButtonType)i);
				return;
			}
		}

		if constexpr(HasButtons<DerivedTabButtonType>)
		{
			for(size_t i = 0; i < tabButtons.size(); i++)
			{
				if(!tabButtons[i].checkCollision(x, y))
					continue;

				if(pressedTabButtonType != DerivedTabButtonType::COUNT)
					tabButtons[(std::size_t)pressedTabButtonType].setPressed(false);

				pressedTabButtonType = (DerivedTabButtonType)i;

				if(pressedTabButtonType != DerivedTabButtonType::COUNT)
				{
					tabButtons[i].setPressed(true);
					static_cast<Derived*>(this)->onTabButtonPressed(pressedTabButtonType);
				}

				return;
			}
		}
	}

	void setTabButtonPressed(DerivedTabButtonType type) requires HasButtons<DerivedTabButtonType>
	{
		if(pressedTabButtonType == type)
			return;

		if(pressedTabButtonType != DerivedTabButtonType::COUNT)
			tabButtons[(size_t)pressedTabButtonType].setPressed(false);

		pressedTabButtonType = type;

		if(type != DerivedTabButtonType::COUNT)
		{
			static_cast<Derived*>(this)->onTabButtonPressed(pressedTabButtonType);
			tabButtons[(size_t)pressedTabButtonType].setPressed(true);
		}
	}

protected:
	std::array<Label, (std::size_t)DerivedButtonType::COUNT> buttons;
	std::array<Label, (std::size_t)DerivedTabButtonType::COUNT> tabButtons;

private:
	Label* hoveredButton{};
	DerivedTabButtonType pressedTabButtonType{DerivedTabButtonType::COUNT};
};