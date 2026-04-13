export module Abrogue:Game;

import :Constants;
import :Configuration;
import :RenderEngine;
import :GUI;
import :Map;
import :Player;
import :EnemyHandler;

import JSON;
import vulkan_hpp;
import ImageLoader;

using namespace std::literals;

/*
 * Game - class for handling game initialization and logic
 */
export class Game
{
public:
    enum class State
    {
        eNotStarted,
        eRunning,
        ePaused,
        eFinished
    };

    [[nodiscard]] bool init();
    [[nodiscard]] bool update();
    void advanceStep() const;

    void startGame();
    void quitToMainMenu();
    void quitToDesktop();

    void setPaused(bool paused);
    void setSpeedPercentage(std::uint64_t speed);
    void setPlayerMovement(std::int64_t movementX, std::int64_t movementY) const;

    [[nodiscard]] bool getShouldExit() const { return state == State::eFinished; }

    void refreshWindowState() const;

private:
    [[nodiscard]] bool updateDraw(double deltaTime) const;

    std::uint64_t guiDeltaTimeNS{};
    std::uint64_t gameDeltaTimeNS{};
    std::uint64_t currentTimeNS{};

    std::uint64_t speedPercentage{100};
    std::uint64_t adjustedTickDuration{Constants::tickDurationNS};

    std::uint64_t framesDrawn{};
    std::uint64_t maxFrameTimeNS{1};
    std::uint64_t lastFPSLogTimeNS{};

    State state{};
};
export inline Game game;
