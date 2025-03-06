module PhysicsComponent;

import Game;

void PhysicsComponent::update()
{
	auto calculateNextStep = [this](double coordinate, double velocity, int32_t movementDirection, int32_t otherMovementDirection)
	{
		std::pair<double, double> result{coordinate, velocity};

		//Advance coordinate by half of time step
		result.first += velocity * Constants::tickDuration / 2.0;

		//Calculate forces
		double movementForce = movementDirection * walkingForce * frictionCoefficient * (otherMovementDirection != 0 ? 1.0 / std::sqrt(2.0) : 1.0);
		double frictionForce{};
		if(std::abs(velocity) > 0.0)
		{
			auto slowSpeed = std::max(0.2 * maxSpeed, std::abs(velocity));
			frictionForce = std::copysign(slowSpeed, -velocity) * frictionCoefficient * resistanceCoefficient;
		}

		//Apply forces and check if speed inverted due to friction
		auto velocitySign = std::signbit(velocity);
		result.second += (movementForce + frictionForce) / mass * Constants::tickDuration / 2.0;
		if(std::signbit(result.second) != velocitySign && movementForce == 0.0)
			result.second = 0.0;

		return result;
	};

	//Do forward euler integration in two steps to reflect coordinate change on same tick
	std::tie(x, velocityX) = calculateNextStep(x, velocityX, movementDirectionX, movementDirectionY);
	std::tie(y, velocityY) = calculateNextStep(y, velocityY, movementDirectionY, movementDirectionX);

	std::tie(x, velocityX) = calculateNextStep(x, velocityX, movementDirectionX, movementDirectionY);
	std::tie(y, velocityY) = calculateNextStep(y, velocityY, movementDirectionY, movementDirectionX);

	auto dx2 = (x - previousX) * (x - previousX);
	auto dy2 = (y - previousY) * (y - previousY);
	if(dx2 < 0.000001 && dy2 < 0.000001)
	{
		previousX = x;
		previousY = y;
		return;
	}

	auto distanceCoefficientX = dx2 < 0.0000001 ? 100000.0 : std::sqrt(1 + dy2 / dx2);
	auto distanceCoefficientY = dy2 < 0.0000001 ? 100000.0 : std::sqrt(4 + dx2 / dy2);

	using TileCoords = std::pair<std::uint32_t, std::uint32_t>;
	struct Collision
	{
		enum Type
		{
			eNone,
			eHorizontal,
			eVertical
		} type{};
		double positionX{}, positionY{};	//Position of tile center during collision
		double distance{std::numeric_limits<double>::max()};
	};

	if(x >= previousX)
	{
		if(y >= previousY)
		{
			auto calculateMinCollision = [&distanceCoefficientX, &distanceCoefficientY](double startX, double startY, double endX, double endY,
																						double offsetX, double offsetY, Collision& minCollision)
			{
				TileCoords startTile{startX, startY};
				TileCoords endTile{endX, endY};
				if(startTile == endTile)
					return;

				std::uint32_t stepsX{}, stepsY{};
				double initialX{(startTile.first + 1 - startX) * distanceCoefficientX};
				double initialY{(startTile.second + 1 - startY) * distanceCoefficientY};
				while(startTile.first < endTile.first || startTile.second < endTile.second)
				{
					double distanceX{initialX + distanceCoefficientX * stepsX}, distanceY{initialY + distanceCoefficientY * stepsY};
					if(distanceX <= distanceY)
					{
						startTile.first++;
						stepsX++;
						if(distanceX >= minCollision.distance)
							return;
						else if(Game::getTileSolid(startTile.first, startTile.second))
						{
							minCollision.type = Collision::eVertical;
							minCollision.positionX = startTile.first - offsetX;
							minCollision.positionY = startTile.second - offsetY;
							minCollision.distance = distanceX;
							return;
						}
					}
					else
					{
						startTile.second++;
						stepsY++;
						if(distanceY >= minCollision.distance)
							return;
						else if(Game::getTileSolid(startTile.first, startTile.second))
						{
							minCollision.type = Collision::eHorizontal;
							minCollision.positionX = startTile.first - offsetX;
							minCollision.positionY = startTile.second - offsetY;
							minCollision.distance = distanceY;
							return;
						}
					}
				}
			};

			Collision minCollision;
			calculateMinCollision((previousX + QuadData::tileScale.x * 0.49f) / QuadData::tileScale.x,
								  (previousY - QuadData::tileScale.y * 0.49f) / QuadData::tileScale.y,
								  (x + QuadData::tileScale.x * 0.49f) / QuadData::tileScale.x,
								  (y - QuadData::tileScale.y * 0.49f) / QuadData::tileScale.y,
								  0.5f, -0.5f,
								  minCollision);
			calculateMinCollision((previousX - QuadData::tileScale.x * 0.49f) / QuadData::tileScale.x,
								  (previousY + QuadData::tileScale.y * 0.49f) / QuadData::tileScale.y,
								  (x - QuadData::tileScale.x * 0.49f) / QuadData::tileScale.x,
								  (y + QuadData::tileScale.y * 0.49f) / QuadData::tileScale.y,
								  -0.5f, 0.5f,
								  minCollision);
			calculateMinCollision((previousX + QuadData::tileScale.x * 0.49f) / QuadData::tileScale.x,
								  (previousY + QuadData::tileScale.y * 0.49f) / QuadData::tileScale.y,
								  (x + QuadData::tileScale.x * 0.49f) / QuadData::tileScale.x,
								  (y + QuadData::tileScale.y * 0.49f) / QuadData::tileScale.y,
								  0.5f, 0.5f,
								  minCollision);

			if(minCollision.type == Collision::eVertical)
			{
				minCollision.type = Collision::eNone;
				minCollision.distance = std::numeric_limits<double>::max();
				velocityX = 0.0;
				x = minCollision.positionX * QuadData::tileScale.x;

				distanceCoefficientX = 100000.0;
				distanceCoefficientY = 1.0;
				calculateMinCollision((x + QuadData::tileScale.x * 0.49f) / QuadData::tileScale.x,
									  minCollision.positionY + 0.49f,
									  (x + QuadData::tileScale.x * 0.49f) / QuadData::tileScale.x,
									  (y + QuadData::tileScale.y * 0.49f) / QuadData::tileScale.y,
									  0.5f, 0.5f, minCollision);
				if(minCollision.type == Collision::eHorizontal)
				{
					velocityY = 0.0;
					y = minCollision.positionY * QuadData::tileScale.y;
				}
			}
			else if(minCollision.type == Collision::eHorizontal)
			{
				minCollision.type = Collision::eNone;
				minCollision.distance = std::numeric_limits<double>::max();

				velocityY = 0.0;
				y = minCollision.positionY * QuadData::tileScale.y;
				distanceCoefficientX = 1.0;
				distanceCoefficientY = 100000.0;
				calculateMinCollision(minCollision.positionX + 0.49f,
									  (y + QuadData::tileScale.y * 0.49f) / QuadData::tileScale.y,
									  (x + QuadData::tileScale.x * 0.49f) / QuadData::tileScale.x,
									  (y + QuadData::tileScale.y * 0.49f) / QuadData::tileScale.y,
									  0.5f, 0.5f, minCollision);
				if(minCollision.type == Collision::eVertical)
				{
					velocityX = 0.0;
					x = minCollision.positionX * QuadData::tileScale.x;
				}
			}
		}
	}

	previousX = x;
	previousY = y;
}