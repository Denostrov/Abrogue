module Abrogue:AnimationHandler;

import :Constants;

class AnimationHandler
{
public:
    AnimationHandler(double endTime) :timeMultiplier(1.0 / endTime) {}

    void update()
    {
        if (isFinished)
            return;

        currentTime += Constants::tickDuration * timeMultiplier * timeDirection;
    }

    double updateDraw(double deltaTime)
    {
        if (isFinished)
            return -1.0;

        double extrapolatedTime = currentTime + deltaTime * timeMultiplier * timeDirection;
        if (extrapolatedTime < 0.0)
        {
            currentTime = 0.0;
            isFinished = true;
            return 0.0;
        }

        if (extrapolatedTime > 1.0)
        {
            currentTime = 1.0;
            isFinished = true;
            return 1.0;
        }

        return extrapolatedTime;
    }

    bool setTimeDirection(bool forward)
    {
        std::int64_t newTimeDirection = forward ? 1 : -1;
        if (newTimeDirection == timeDirection)
            return false;

        timeDirection = newTimeDirection;
        isFinished = false;
        return true;
    }

    [[nodiscard]] auto getIsFinished() const { return isFinished; }
    [[nodiscard]] auto getCurrentDirection() const { return timeDirection; }

private:
    double currentTime{};
    double timeMultiplier{1.0};
    std::int64_t timeDirection{-1};
    bool isFinished{true};
};
