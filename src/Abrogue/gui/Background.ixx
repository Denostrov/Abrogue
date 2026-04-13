module Abrogue:Background;

import :QuadPool;
import :AnimationHandler;

/*
 * Class for a semi transparent rectangle that covers the whole screen
 * The rectangle fades in over time after being shown
 */
class Background
{
public:
    void update()
    {
        if (animationHandler.getIsFinished())
            return;

        auto result = animationHandler.update();
        if (result == 0 && !animationHandler.getIsForward())
            quad.clear();
    }

    void updateDraw(double deltaTime)
    {
        if (animationHandler.getIsFinished())
            return;

        auto extrapolatedTime = animationHandler.updateDraw(deltaTime);
        auto currentColor = extrapolatedTime * 240.0;
        quad.setBackgroundColor(Color::pack(0, 0, 0, currentColor));
    }

    void setVisible(bool visible)
    {
        if (animationHandler.getIsForward() == visible)
            return;

        animationHandler.startAnimation(visible);
        if (!quad)
        {
            quad.init(QuadData{
                {Constants::screenWidth / 2.0f, Constants::screenHeight / 2.0f},
                {Color::pack(0, 0, 0, 0), Color::pack(0, 0, 0, 0)}, ' ',
                {Constants::screenWidth, Constants::screenHeight}
            });
        }
    }

private:
    AnimationHandler animationHandler{100'000'000};
    QuadReference<QuadLayer::ePopupBackground> quad;
};
