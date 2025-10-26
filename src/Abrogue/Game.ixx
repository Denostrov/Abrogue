module;

#include "vulkan/vk_platform.h"

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

    bool init();
    bool update();
    void advanceStep();

    void startGame();
    void quitToMainMenu();
    void quitToDesktop();

    void setPaused(bool paused);
    void setSpeedMultiplier(double speed);
    void setPlayerMovement(std::int64_t movementX, std::int64_t movementY);

    [[nodiscard]] bool getShouldExit() const { return state == State::eFinished; }

    void refreshWindowState() const;

private:
    void resetTickTimer();

    bool updateDraw(double deltaTime) const;

    std::uint64_t currentTick{};
    std::uint64_t lastUpdateTime{};
    double speedMultiplier{1.0};

    std::uint64_t framesDrawn{};
    std::uint64_t lastFPSLogTime{};

    State state{};
};
export inline Game game;

/*
 * Implementation of Game methods
 */
bool Game::init()
{
    if (!logger.init())
        return false;

    if (!configuration.init())
        return false;

    if (!renderEngine.init())
        return false;

    gui.init();

    lastUpdateTime = SDL_GetTicksNS();
    lastFPSLogTime = lastUpdateTime;

    resetTickTimer();

    return true;
}
bool Game::update()
{
    std::uint64_t currentTime = SDL_GetTicksNS();
    std::uint64_t updateCount{};
    while (static_cast<double>(currentTime - lastUpdateTime) * speedMultiplier > Constants::tickDurationNS)
    {
        advanceStep();

        lastUpdateTime += static_cast<std::uint64_t>(Constants::tickDurationNS / speedMultiplier);

        updateCount++;
        if (updateCount > 4)
        {
            lastUpdateTime = currentTime;
            logger.logInfo("Can't keep up, skipping ticks");
            break;
        }
    }

    double deltaTimeInSec = static_cast<double>(currentTime - lastUpdateTime) / 1.e9;
    if (!updateDraw(deltaTimeInSec * speedMultiplier))
        return false;

    framesDrawn++;
    if (auto timeSinceLastLog = static_cast<double>(currentTime - lastFPSLogTime) / 1.e9; timeSinceLastLog > 1.0)
    {
        auto fps = static_cast<double>(framesDrawn) / timeSinceLastLog;
        gui.setFPS(static_cast<std::uint32_t>(fps));

        framesDrawn = 0;
        lastFPSLogTime = currentTime;
    }

    return true;
}
void Game::advanceStep()
{
    gui.update();
    if (state == State::eRunning)
    {
        currentTick++;

        player.update();
        map.update();
        enemyHandler.update();
    }
}
void Game::startGame()
{
    lastUpdateTime = SDL_GetTicksNS();
    mapRandom.seed(lastUpdateTime);
    visualRandom.seed(lastUpdateTime);

    player = Player(10.0);
    map.init();

    state = State::eRunning;

    gui.showPlayArea();

    resetTickTimer();
}
void Game::quitToDesktop()
{
    state = State::eFinished;
}
void Game::setPaused(bool paused)
{
    state = paused ? State::ePaused : State::eRunning;
}
void Game::setSpeedMultiplier(double speed)
{
    logger.extraAssert(speed >= 0.0, "Set incorrect speed multiplier");

    speedMultiplier = speed;
    lastUpdateTime = SDL_GetTicksNS();
}
void Game::setPlayerMovement(std::int64_t movementX, std::int64_t movementY)
{
    if (state != State::eRunning && state != State::ePaused)
        return;

    player.setMovement(movementX, movementY);
}
void Game::quitToMainMenu()
{
    gui.showMainMenu();

    state = State::eNotStarted;
    map = Map();
    player = Player();
    enemyHandler = EnemyHandler();
}
void Game::refreshWindowState() const
{
    gui.refreshScreens();
    configuration.updateWindowOptions();
}
void Game::resetTickTimer()
{
    currentTick = 0;
}
bool Game::updateDraw(double deltaTime) const
{
    gui.updateDraw(deltaTime);
    if (state == State::eRunning)
    {
        player.updateDraw(deltaTime);

        map.updateDraw(deltaTime);

        enemyHandler.updateDraw(deltaTime);
    }

    return renderEngine.drawFrame();
}
