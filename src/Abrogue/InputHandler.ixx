export module Abrogue:InputHandler;

import :Constants;

/*
 * InputHandler - class for handling user input
 */
export class InputHandler
{
public:
    InputHandler() = default;

    void setChangingControlType(InputControlType type) { changingControlType = type; }

    [[nodiscard]] std::pair<float, float> getMousePosition() const;
    void onMouseMoved(float x, float y);
    void onMousePressed(std::uint8_t buttonIndex, float x, float y);

    void onButtonPressed(SDL_Scancode scancode, bool pressed);

private:
    std::array<bool, SDL_SCANCODE_COUNT> pressedButtons{};
    InputControlType changingControlType{InputControlType::COUNT};
};
export inline InputHandler inputHandler;