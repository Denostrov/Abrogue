module Abrogue:Background;

import :QuadPool;

/*
 * Class for a semi transparent rectangle that covers the whole screen
 * The rectangle fades in over time after being shown
 */
class Background
{
public:
    void updateDraw(double deltaTime)
    {
        if (!quad || animationTime > animationEndTime)
            return;

        animationTime += deltaTime;
        quad.setBackgroundColor(Color::pack(0, 0, 0, std::min(animationTime / animationEndTime * 240.0, 240.0)));
    }
    void setVisible(bool visible)
    {
        if (!visible)
        {
            quad.clearData();
            return;
        }

        if (quad)
            return;

        animationTime = 0.0;

        quad.setData(QuadData{
            {Constants::screenWidth / 2.0f, Constants::screenHeight / 2.0f},
            {Color::pack(0, 0, 0, 0), Color::pack(0, 0, 0, 0)}, ' ',
            {Constants::screenWidth, Constants::screenHeight}
        });
    }

private:
    static constexpr double animationEndTime{2.0};

    double animationTime{};
    QuadReference<QuadLayer::ePopupBackground> quad;
};
