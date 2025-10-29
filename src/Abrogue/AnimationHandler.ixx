module Abrogue:AnimationHandler;

import :Constants;

class AnimationHandler
{
public:
    AnimationHandler(double endTime) :endTime(endTime) {}

    void update()
    {
        if (isFinished)
            return;

        currentTime += Constants::tickDuration * timeDirection;
    }

    std::pair<bool, double> updateDraw(double deltaTime)
    {
        if (isFinished)
            return {false, currentTime / endTime};

        double extrapolatedTime = (currentTime + deltaTime * timeDirection) / endTime;
        if (extrapolatedTime < 0.0)
        {
            currentTime = 0.0;
            isFinished = true;
            return {true, currentTime / endTime};
        }

        if (extrapolatedTime > 1.0)
        {
            currentTime = endTime;
            isFinished = true;
            return {true, currentTime / endTime};
        }

        return {true, extrapolatedTime};
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

    [[nodiscard]] bool getIsFinished() const { return isFinished; }

private:
    double currentTime{};
    double endTime{1.0};
    std::int64_t timeDirection{-1};
    bool isFinished{true};
};
