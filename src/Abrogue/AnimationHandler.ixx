module Abrogue:AnimationHandler;

import :Constants;

class AnimationHandler
{
public:
    AnimationHandler(std::uint64_t endTimeNS) : endTimeNS(endTimeNS), endTime(endTimeNS / 1.e9) {}

    std::uint64_t update()
    {
        logger.extraAssert(!isFinished, "AnimationHandler updated after finishing");

        currentTimeNS += Constants::tickDurationNS * timeDirection;
        currentTime = currentTimeNS / 1.e9;
        if (currentTimeNS > endTimeNS)
        {
            currentTimeNS = endTimeNS;
            isFinished = true;
        }
        else if (currentTimeNS < 0)
        {
            currentTimeNS = 0;
            isFinished = true;
        }

        return currentTimeNS * 1000 / endTimeNS;
    }

    double updateDraw(double deltaTime)
    {
        logger.extraAssert(!isFinished, "AnimationHandler updated draw after finishing");

        double extrapolatedTime = (currentTime + deltaTime * timeDirection) / endTime;
        return std::clamp(extrapolatedTime, 0.0, 1.0);
    }

    void startAnimation(bool forward)
    {
        std::int64_t newTimeDirection = forward ? 1 : -1;
        logger.extraAssert(newTimeDirection != timeDirection, "AnimationHandler started animation in same direction");

        timeDirection = newTimeDirection;
        isFinished = false;
    }

    [[nodiscard]] auto getIsFinished() const { return isFinished; }
    [[nodiscard]] auto getIsForward() const { return timeDirection == 1; }

private:
    std::int64_t endTimeNS{};
    double endTime{};

    std::int64_t currentTimeNS{};
    double currentTime{};

    std::int64_t timeDirection{-1};
    bool isFinished{true};
};
