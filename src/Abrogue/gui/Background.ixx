module Abrogue:Background;

import :QuadPool;

/*
 * Class for a semi transparent rectangle that covers the whole screen
 * The rectangle fades in over time after being shown
 */
class Background
{
public:
    void update()
    {
        if (isAnimationFinished)
            return;

        animationTime += Constants::tickDuration * fadingDirection;
    }
    void updateDraw(double deltaTime)
    {
        if (isAnimationFinished)
            return;

        double currentColor = (animationTime + deltaTime * fadingDirection) / animationEndTime * 240.0;
        if (currentColor < 0.0)
        {
            animationTime = 0.0;
            isAnimationFinished = true;
            quad.clearData();
            return;
        }

        if (currentColor > 240.0)
        {
            currentColor = 240.0;
            animationTime = animationEndTime;
            isAnimationFinished = true;
        }
        quad.setBackgroundColor(Color::pack(0, 0, 0, currentColor));
    }
    void setVisible(bool visible)
    {
        std::int64_t newFadingDirection = visible ? 1 : -1;
        if (newFadingDirection == fadingDirection)
            return;

        fadingDirection = newFadingDirection;
        isAnimationFinished = false;
        if (!quad)
        {
            quad.setData(QuadData{
                {Constants::screenWidth / 2.0f, Constants::screenHeight / 2.0f},
                {Color::pack(0, 0, 0, 0), Color::pack(0, 0, 0, 0)}, ' ',
                {Constants::screenWidth, Constants::screenHeight}
            });
        }
    }

private:
    static constexpr double animationEndTime{0.05};

    std::int64_t fadingDirection{-1};
    bool isAnimationFinished{true};
    double animationTime{};
    QuadReference<QuadLayer::ePopupBackground> quad;
};
