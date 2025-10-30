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
        animationHandler.update();
    }
    void updateDraw(double deltaTime)
    {
        auto extrapolatedTime = animationHandler.updateDraw(deltaTime);
        if (extrapolatedTime <= -1.0)
            return;

        if (extrapolatedTime <= 0.0 && animationHandler.getCurrentDirection() == -1)
        {
            quad.clearData();
            return;
        }

        double currentColor = extrapolatedTime * 240.0;
        quad.setBackgroundColor(Color::pack(0, 0, 0, currentColor));
    }
    void setVisible(bool visible)
    {
        if (!animationHandler.setTimeDirection(visible))
            return;

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
    AnimationHandler animationHandler{0.1};
    QuadReference<QuadLayer::ePopupBackground> quad;
};
