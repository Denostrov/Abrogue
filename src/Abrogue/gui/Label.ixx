module Abrogue:Label;

import :QuadPool;
import :AnimationHandler;

//Class for handling clickable text boxes
template <QuadLayer layer>
class Label
{
public:
    Label() = default;

    void init(std::string_view text, std::int64_t x, std::int64_t y, bool visible = false)
    {
        setPosition(x, y);
        setText(text);

        setVisible(visible);
    }

    void update()
    {
        if (animationHandler.getIsFinished())
            return;

        animationHandler.update();
    }
    void updateDraw(double deltaTime)
    {
        if (animationHandler.getIsFinished())
            return;

        auto extrapolatedTime = animationHandler.updateDraw(deltaTime);
        if (extrapolatedTime <= -1.0)
            return;

        if (extrapolatedTime <= 0.0 && !animationHandler.getIsForward())
        {
            quadReferences.clear();
            return;
        }

        for (std::size_t i = 0; i < quadReferences.getSize(); i++)
        {
            quadReferences[i].setPosition(x + i + 0.5f, y + 0.5f + Constants::mapHeight * (1.0 - extrapolatedTime));
        }
    }

    [[nodiscard]] bool checkCollision(std::int64_t checkX, std::int64_t checkY) const
    {
        return x <= checkX && checkX < x + size && y <= checkY && checkY < y + 1;
    }

    void togglePressed() { setPressed(!isPressed); }

    [[nodiscard]] auto getPressed() const { return isPressed; }

    void setVisible(bool visible)
    {
        if (animationHandler.getIsForward() == visible)
            return;

        animationHandler.startAnimation(visible);

        //Invisible label has size 0 to disable collision detection
        isVisible = visible;
        size = visible ? text.getSize() : 0;

        //Delete invisible quads to avoid overdraw
        if (!quadReferences.isEmpty())
            return;

        //Recreate quads
        for (std::size_t i = 0; i < size; i++)
        {
            quadReferences.emplaceBack();
            quadReferences.getBack().init(QuadData{
                {x + i + 0.5f, y + 0.5f},
                {Color::pack(255, 255, 255, 255), getBackgroundColor(i)}, (std::uint32_t)text[i]
            });
        }
    }
    void setHovered(bool hovered)
    {
        if (isHovered == hovered)
            return;

        isHovered = hovered;

        for (std::size_t i = 0; i < size; i++)
            quadReferences[i].setBackgroundColor(getBackgroundColor(i));
    }
    void setPressed(bool pressed)
    {
        if (isPressed == pressed)
            return;

        isPressed = pressed;

        for (std::size_t i = 0; i < size; i++)
            quadReferences[i].setBackgroundColor(getBackgroundColor(i));
    }

    void setText(std::string_view newText)
    {
        text = newText;
        if (!isVisible)
            return;

        size = text.getSize();

        //Create remaining quads when new text is longer
        for (std::size_t i = quadReferences.getSize(); i < size; ++i)
        {
            quadReferences.emplaceBack();
            quadReferences.getBack().init(QuadData{
                {x + i + 0.5f, y + 0.5f},
                {Color::pack(255, 255, 255, 255), getBackgroundColor(i)}, (std::uint32_t)text[i]
            });
        }

        //Set existing quad parameters
        for (std::size_t i = 0; i < size; i++)
        {
            quadReferences[i].setGlyph(text[i]);
            quadReferences[i].setBackgroundColor(getBackgroundColor(i));
        }

        //Erase extra quads when new text is shorter
        quadReferences.resize(size);
    }
    void setPosition(std::int64_t newX, std::int64_t newY)
    {
        x = newX;
        y = newY;
        for (std::size_t i = 0; i < size; i++)
            quadReferences[i].setPosition(x + i + 0.5f, y + 0.5f);
    }
    void setBackgroundColor(PackedColor color, PackedColor hoverColor)
    {
        backgroundColor = color;
        hoveredBackgroundColor = hoverColor;

        for (std::size_t i = 0; i < size; i++)
            quadReferences[i].setBackgroundColor(getBackgroundColor(i));
    }
    void setPressedBackgroundColor(PackedColor color, PackedColor hoverColor)
    {
        pressedBackgroundColor = color;
        hoveredPressedBackgroundColor = hoverColor;

        for (std::size_t i = 0; i < size; i++)
            quadReferences[i].setBackgroundColor(getBackgroundColor(i));
    }
    void setProgress(double percentage)
    {
        progress = percentage;

        for (std::size_t i = 0; i < size; i++)
            quadReferences[i].setBackgroundColor(getBackgroundColor(i));
    }

private:
    [[nodiscard]] PackedColor getBackgroundColor(std::int64_t index) const
    {
        Color color = isPressed
                          ? (isHovered ? hoveredPressedBackgroundColor : pressedBackgroundColor)
                          : (isHovered ? hoveredBackgroundColor : backgroundColor);

        //Adjust colors to form a gradient from left to right depending on progress
        double colorCoefficient = std::clamp(progress * size - index, 0.0, 1.0);
        color.multiplyRGB(colorCoefficient);
        return color.getPacked();
    }

    AnimationHandler animationHandler{2'000'000'000};

    bool isVisible{};
    bool isHovered{};
    bool isPressed{};
    double progress{1.0};

    std::int64_t x{}, y{};
    std::size_t size{};

    PackedColor backgroundColor{Constants::labelBackgroundColor};
    PackedColor hoveredBackgroundColor{Constants::labelHoveredColor};
    PackedColor pressedBackgroundColor{Constants::labelPressedColor};
    PackedColor hoveredPressedBackgroundColor{Constants::labelHoveredPressedColor};

    FixedString<128> text;
    FixedVector<QuadReference<layer>, 128> quadReferences;
};
