export module Button;

export import Label;
export import Event;

export class ButtonPressedEvent: public Event<ButtonPressedEvent>
{
public:
	enum Type
	{
		eStartGame,
		eContinue,
		eExit
	}type;
};

export class Button
{
public:
	Button() = default;
	Button(std::string_view text, ButtonPressedEvent::Type pressedType, std::uint32_t x, std::uint32_t y)
		:label(text, x, y), pressedType(pressedType)
	{

	}

private:
	Label label;
	ButtonPressedEvent::Type pressedType{};
};