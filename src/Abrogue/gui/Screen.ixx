export module Screen;

export import Label;
export import Logger;

using namespace std::literals;

export enum class EmptyEnumType
{
	COUNT
};

template<class T>
concept HasButtons = (std::size_t)T::COUNT > 0;

//Base class for GUI screens
export
template<class Derived,
	class DerivedLabelType = EmptyEnumType,
	class DerivedButtonType = EmptyEnumType,
	class DerivedTabButtonType = EmptyEnumType>
class ScreenComponent
{
public:
	using LabelType = DerivedLabelType;
	using ButtonType = DerivedButtonType;
	using TabButtonType = DerivedTabButtonType;

	ScreenComponent() = default;

	void setVisible(bool visible)
	{
		if constexpr(HasButtons<LabelType>)
		{
			for(auto& label : labels)
				label.setVisible(visible);
		}

		if constexpr(HasButtons<ButtonType>)
		{
			for(auto& button : buttons)
				button.setVisible(visible);
		}

		if constexpr(HasButtons<TabButtonType>)
		{
			for(auto& tabButton : tabButtons)
				tabButton.setVisible(visible);
		}
	}

	void updateMouseMoved(std::int64_t x, std::int64_t y)
	{
		auto handleButtonCollisions = [this, x, y](auto& buttons)
		{
			for(std::size_t i = 0; i < buttons.size(); i++)
			{
				if(!buttons[i].checkCollision(x, y))
					continue;

				auto button = &buttons[i];
				if(button == hoveredButton)
					return true;

				if(hoveredButton)
					hoveredButton->setHovered(false);

				button->setHovered(true);
				hoveredButton = button;
				return true;
			}

			return false;
		};

		if constexpr(HasButtons<ButtonType>)
		{
			if(handleButtonCollisions(buttons))
				return;
		}

		if constexpr(HasButtons<TabButtonType>)
		{
			if(handleButtonCollisions(tabButtons))
				return;
		}

		if(!hoveredButton)
			return;

		hoveredButton->setHovered(false);
		hoveredButton = nullptr;
	}
	void updateMousePressed(std::int64_t x, std::int64_t y)
	{
		if constexpr(HasButtons<ButtonType>)
		{
			for(size_t i = 0; i < buttons.size(); i++)
			{
				if(!buttons[i].checkCollision(x, y))
					continue;

				static_cast<Derived*>(this)->onButtonPressed((ButtonType)i);
				return;
			}
		}

		if constexpr(HasButtons<TabButtonType>)
		{
			for(size_t i = 0; i < tabButtons.size(); i++)
			{
				if(!tabButtons[i].checkCollision(x, y))
					continue;

				if(pressedTabButtonType != TabButtonType::COUNT)
					tabButtons[pressedTabButtonType].setPressed(false);

				pressedTabButtonType = (TabButtonType)i;

				if(pressedTabButtonType != TabButtonType::COUNT)
				{
					tabButtons[i].setPressed(true);
					static_cast<Derived*>(this)->onTabButtonPressed(pressedTabButtonType);
				}

				return;
			}
		}
	}

	void pressButton(ButtonType type) requires HasButtons<ButtonType>
	{
		logger.extraAssert(type < ButtonType::COUNT, "invalid button type"sv);

		static_cast<Derived*>(this)->onButtonPressed(type);
	}

	void setTabButtonPressed(TabButtonType type) requires HasButtons<TabButtonType>
	{
		if(pressedTabButtonType == type)
			return;

		if(pressedTabButtonType != TabButtonType::COUNT)
			tabButtons[pressedTabButtonType].setPressed(false);

		pressedTabButtonType = type;

		if(type != TabButtonType::COUNT)
		{
			static_cast<Derived*>(this)->onTabButtonPressed(pressedTabButtonType);
			tabButtons[pressedTabButtonType].setPressed(true);
		}
	}

protected:
	Array<Label, LabelType::COUNT> labels;
	Array<Label, ButtonType::COUNT> buttons;
	Array<Label, TabButtonType::COUNT> tabButtons;

private:
	Label* hoveredButton{};
	TabButtonType pressedTabButtonType{TabButtonType::COUNT};
};