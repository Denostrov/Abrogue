export module Screen;

export import Label;

export class Screen
{
public:
	Screen() = default;

	virtual void setVisible(bool visible) {}

	void recheckHoverPosition();

	void updateMouseMoved(std::uint32_t x, std::uint32_t y);
	void updateMousePressed(std::uint32_t x, std::uint32_t y);

protected:
	std::span<Label> pressableButtons;

private:
	virtual void onButtonPressed(size_t index) {}

	Label* hoveredButton{};
};