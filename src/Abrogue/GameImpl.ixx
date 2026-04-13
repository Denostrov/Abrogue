module Abrogue:GameImpl;

import :Game;

bool Game::init()
{
    if (!logger.init())
        return false;

    if (!configuration.init())
        return false;

    if (!renderEngine.init())
        return false;

    gui.init();

    currentTimeNS = SDL_GetTicksNS();
    lastFPSLogTimeNS = currentTimeNS;

    return true;
}

bool Game::update()
{
    auto newTimeNS = SDL_GetTicksNS();
    auto deltaTimeNS = newTimeNS - currentTimeNS;
    if (deltaTimeNS > maxFrameTimeNS)
        maxFrameTimeNS = deltaTimeNS;
    gameDeltaTimeNS += deltaTimeNS;
    guiDeltaTimeNS += deltaTimeNS;
    currentTimeNS = newTimeNS;

    std::uint64_t updateCount{};
    while (guiDeltaTimeNS >= Constants::tickDurationNS)
    {
        gui.update();

        guiDeltaTimeNS -= Constants::tickDurationNS;

        updateCount++;
        if (updateCount > 4)
        {
            guiDeltaTimeNS = 0;
            logger.logInfo("Can't keep up with gui, skipping ticks");
            break;
        }
    }
    gui.updateDraw(guiDeltaTimeNS / 1.e9);

    updateCount = 0;
    while (gameDeltaTimeNS >= adjustedTickDuration)
    {
        advanceStep();

        gameDeltaTimeNS -= adjustedTickDuration;

        updateCount++;
        if (updateCount > 4)
        {
            gameDeltaTimeNS = 0;
            logger.logInfo("Can't keep up with game, skipping ticks");
            break;
        }
    }
    if (!updateDraw(gameDeltaTimeNS / 1.e9 * speedPercentage / 100))
        return false;

    framesDrawn++;
    if (auto timeSinceLastLog = currentTimeNS - lastFPSLogTimeNS; timeSinceLastLog >= 1'000'000'000)
    {
        auto fps = framesDrawn * 10'000'000'000 / timeSinceLastLog;
        fps = fps / 10 + (fps % 10 >= 5);
        gui.setFPS(fps, timeSinceLastLog / maxFrameTimeNS);

        framesDrawn = 0;
        maxFrameTimeNS = 1;
        lastFPSLogTimeNS = currentTimeNS;
    }

    return true;
}

void Game::advanceStep() const
{
    if (state != State::eRunning)
        return;

    player.update();
    map.update();
    enemyHandler.update();
}

void Game::startGame()
{
    currentTimeNS = SDL_GetTicksNS();
    mapRandom.seed(currentTimeNS);
    visualRandom.seed(currentTimeNS);

    player = Player(10.0);
    map.init();

    state = State::eRunning;

    gui.showPlayArea();
}

void Game::quitToDesktop()
{
    state = State::eFinished;
}

void Game::setPaused(bool paused)
{
    state = paused ? State::ePaused : State::eRunning;
}

void Game::setSpeedPercentage(std::uint64_t speed)
{
    logger.extraAssert(speed <= 1'000'000, "Set incorrect speed multiplier");

    speedPercentage = speed;
    gameDeltaTimeNS = 0;
    adjustedTickDuration = speedPercentage != 0 ? Constants::tickDurationNS * 100 / speedPercentage : std::numeric_limits<std::uint64_t>::max();
}

void Game::setPlayerMovement(std::int64_t movementX, std::int64_t movementY) const
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

bool Game::updateDraw(double deltaTime) const
{
    if (state == State::eRunning)
    {
        player.updateDraw(deltaTime);

        map.updateDraw(deltaTime);

        enemyHandler.updateDraw(deltaTime);
    }

    return renderEngine.drawFrame();
}