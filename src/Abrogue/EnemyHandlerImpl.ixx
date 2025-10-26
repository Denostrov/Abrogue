module Abrogue:EnemyHandlerImpl;

import :EnemyHandler;
import :Map;
import :Player;
import :Configuration;

template <bool isDebug>
EnemyHandler::Enemy::Enemy(EnemyData const& data, double positionX, double positionY, State initialState, BoolSequence<isDebug>)
    : PhysicsComponent(positionX, positionY, 0.45, 0.45, 0.45, 0.45), color(data.color), state(initialState)
{
    auto [x, y] = getPosition();
    quad.setData(QuadData{
        {Constants::mapOffset + x, y},
        {color.getPacked(), color.getTransparentPacked()}, data.symbol
    });

    if constexpr (isDebug)
        updateDrawDebug();

    setMass(data.mass);
    setMaxVelocity(data.speed);

    weapon.init(data.weaponType, data.weaponColor, data.damage, data.attackTime, false);
}

template <bool isDebug>
bool EnemyHandler::spawnEnemy()
{
    auto enemyDataOpt = configuration.getSuitableEnemy();
    if (!enemyDataOpt)
        return false;

    auto const& spawnRoom = map.getRandomRoom();
    std::int64_t spawnX = spawnRoom.originX + mapRandom.generate() % spawnRoom.width;
    std::int64_t spawnY = spawnRoom.originY + mapRandom.generate() % spawnRoom.height;

    if (map.getTileInLineOfSight(spawnX, spawnY))
        return false;

    enemies.emplaceBack(*enemyDataOpt, spawnX + 0.5, spawnY + 0.5, Enemy::State::eSleeping, BoolSequence<isDebug>{});

    return true;
}

void EnemyHandler::Enemy::update(double playerX, double playerY, double playerVelocityX, double playerVelocityY, std::int64_t stealthRange)
{
    auto [x, y] = getPosition();

    double distanceX = (playerX - x) / 2.0;
    double distanceY = playerY - y;
    double totalDistance = std::sqrt(distanceX * distanceX + distanceY * distanceY);

    auto moveTowards = [this, x, y](double targetX, double targetY)
    {
        double distanceX = (targetX - x) / 2.0;
        double distanceY = targetY - y;
        if (std::abs(distanceX) > std::abs(distanceY))
            setMovementDirection(std::abs(distanceX) < 0.05 ? 0.0 : std::copysign(1.0, distanceX),
                                 std::abs(distanceY) < 0.05 ? 0.0 : std::copysign(0.5, distanceY));
        else
            setMovementDirection(std::abs(distanceX) < 0.05 ? 0.0 : std::copysign(0.5, distanceX),
                                 std::abs(distanceY) < 0.05 ? 0.0 : std::copysign(1.0, distanceY));
    };

    if (state == State::eHunting)
    {
        auto [targetTileX, targetTileY] = path[currentPathIndex];

        if ((std::int64_t)x != targetTileX || (std::int64_t)y != targetTileY)
        {
            moveTowards(targetTileX + 0.5, targetTileY + 0.5);
        }
        else
        {
            currentPathIndex++;
            if (currentPathIndex >= path.getSize())
                setState(State::eWandering);
        }
    }
    else if (state == State::eSleeping)
    {
        if (totalDistance < stealthRange && map.getTileInLineOfSight(x, y))
        {
            stealthTimer += Constants::tickDuration;
            if (stealthTimer > lastCheckedStealthTime + 0.5)
            {
                lastCheckedStealthTime += 0.5;
                std::uint64_t detectRoll = mapRandom.generate() % 4;
                if (detectRoll == 0)
                {
                    setState(State::eHunting);
                    stealthTimer = 0.0;
                    lastCheckedStealthTime = 0.0;
                    setPathTo(playerX, playerY);
                }
            }
        }
    }
    else if (state == State::eWandering)
    {
        if (totalDistance < stealthRange && map.getTileInLineOfSight(x, y))
        {
            if ((std::int64_t)x != (std::int64_t)playerX || (std::int64_t)y != (std::int64_t)playerY)
            {
                setState(State::eHunting);
                setPathTo(playerX, playerY);
            }
        }
    }

    PhysicsComponent::update();

    std::tie(x, y) = getPosition();
    weapon.update(x, y);
}

void EnemyHandler::Enemy::updateDraw(double deltaTime)
{
    auto [x, y] = getPosition();
    auto [vx, vy] = getVelocity();
    quad.setPosition(Constants::mapOffset + x + vx * deltaTime, y + vy * deltaTime);

    if (enemyHandler.isDrawDebug)
        stateQuad.setPosition(Constants::mapOffset + x + vx * deltaTime - 0.25, y + vy * deltaTime + 0.25);

    auto brightness = map.getTileBrightness(x, y);
    if (brightness < Constants::mapMinBrightness)
    {
        quad.setColor(0);
        quad.setBackgroundColor(0);
    }
    else
    {
        quad.setColor(Color::pack(color.r * brightness, color.g * brightness, color.b * brightness, color.a));
        quad.setBackgroundColor(Color::pack(color.r * brightness, color.g * brightness, color.b * brightness, 0));

        weapon.updateDraw(Constants::mapOffset + x, y);
    }
}

void EnemyHandler::Enemy::updateDrawDebug()
{
    pathQuads.clear();
    stateQuad = {};
    if (enemyHandler.isDrawDebug)
    {
        for (auto [pathX, pathY] : path)
        {
            pathQuads.emplaceBack(QuadData{
                {Constants::mapOffset + pathX + 0.5, pathY + 0.5},
                {Color::pack(0, 0, 0, 0), Color::pack(255, 0, 0, 128)}, ' '
            });
        }

        auto [x, y] = getPosition();
        QuadData stateData{{Constants::mapOffset + x - 0.25, y + 0.25}, {Color::pack(255, 0, 0, 255), Color::pack(255, 0, 0, 0)}, 'S'};
        stateData.setScale(0.5, 0.5);
        stateQuad.setData(stateData);
        stateQuad.setGlyph(state == State::eSleeping ? 'S' : state == State::eWandering ? 'W' : state == State::eHunting ? 'H' : '?');
    }
}

void EnemyHandler::Enemy::setState(State newState)
{
    state = newState;
    if (enemyHandler.isDrawDebug)
    {
        stateQuad.setGlyph(state == State::eSleeping ? 'S' : state == State::eWandering ? 'W' : state == State::eHunting ? 'H' : '?');
    }
}

void EnemyHandler::Enemy::setPathTo(std::int64_t x, std::int64_t y)
{
    auto [currentX, currentY] = getPosition();

    path = map.getPath(currentX, currentY, x, y);
    currentPathIndex = 0;

    updateDrawDebug();
}

void EnemyHandler::update()
{
    currentTime += Constants::tickDuration;

    if (currentTime - lastEnemySpawnTime > 60.0)
    {
        spawnEnemy<false>();

        lastEnemySpawnTime = currentTime;
    }

    auto [playerX, playerY] = player.getPosition();
    auto [playerVelocityX, playerVelocityY] = player.getVelocity();
    auto stealthRange = player.getStealthRange();
    for (auto& enemy : enemies)
        enemy.update(playerX, playerY, playerVelocityX, playerVelocityY, stealthRange);
}

void EnemyHandler::updateDraw(double deltaTime)
{
    for (auto& enemy : enemies)
        enemy.updateDraw(deltaTime);
}

void EnemyHandler::inflictDamage(double damageX, double damageY)
{
    for (std::size_t i = 0; i < enemies.getSize(); i++)
    {
        auto [enemyX, enemyY] = enemies[i].getPosition();
        if (damageX > enemyX - 0.6 && damageX < enemyX + 0.6 && damageY > enemyY - 0.6 && damageY < enemyY + 0.6)
        {
            enemies.erase(i);
            i--;
        }
    }
}

void EnemyHandler::populateLevel()
{
    auto spawnEnemies = [this]<bool isDebug>(BoolSequence<isDebug>)
    {
        for (std::int64_t i = 0; i < 20; i++)
        {
            if (!spawnEnemy<isDebug>())
                i--;
        }
    };

    BoolFlag::call(spawnEnemies, std::array{isDrawDebug});
}

void EnemyHandler::setDrawDebug(bool draw)
{
    enemyHandler.isDrawDebug = draw;
    for (auto& enemy : enemies)
        enemy.updateDrawDebug();
}
