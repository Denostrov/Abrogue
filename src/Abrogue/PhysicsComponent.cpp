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

	if(x == previousX && y == previousY)
		return;

	auto dx2 = (x - previousX) * (x - previousX);
	auto dy2 = (y - previousY) * (y - previousY);

	auto distanceCoefficientX = dx2 < 1.e-10 ? 100000.0 : std::sqrt(1 + dy2 / dx2);
	auto distanceCoefficientY = dy2 < 1.e-10 ? 100000.0 : std::sqrt(4 + 4 * dx2 / dy2);

	using TileCoords = std::pair<std::int32_t, std::int32_t>;
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

	std::int32_t directionX = x >= previousX ? 1 : -1;
	std::int32_t directionY = y >= previousY ? 1 : -1;

	double previousPositiveX = (previousX + QuadData::tileScale.x * 0.49f * directionX) / QuadData::tileScale.x;
	double positiveX = (x + QuadData::tileScale.x * 0.49f * directionX) / QuadData::tileScale.x;

	double previousNegativeX = (previousX - QuadData::tileScale.x * 0.49f * directionX) / QuadData::tileScale.x;
	double negativeX = (x - QuadData::tileScale.x * 0.49f * directionX) / QuadData::tileScale.x;

	double previousPositiveY = (previousY + QuadData::tileScale.y * 0.49f * directionY) / QuadData::tileScale.y;
	double positiveY = (y + QuadData::tileScale.y * 0.49f * directionY) / QuadData::tileScale.y;

	double previousNegativeY = (previousY - QuadData::tileScale.y * 0.49f * directionY) / QuadData::tileScale.y;
	double negativeY = (y - QuadData::tileScale.y * 0.49f * directionY) / QuadData::tileScale.y;

	auto calculateMinCollision = [&distanceCoefficientX, &distanceCoefficientY, directionX, directionY](double startX, double startY, double endX, double endY,
																										Collision& minCollision)
	{
		TileCoords startTile{startX, startY};
		TileCoords endTile{endX, endY};
		if(startTile == endTile)
			return;

		auto totalStepsX = std::abs(startTile.first - endTile.first);
		auto totalStepsY = std::abs(startTile.second - endTile.second);

		std::uint32_t stepsX{}, stepsY{};
		double initialX{std::abs((directionX == 1) - (startX - startTile.first)) * distanceCoefficientX};
		double initialY{std::abs((directionY == 1) - (startY - startTile.second)) * distanceCoefficientY};
		while(totalStepsX > 0 || totalStepsY > 0)
		{
			double distanceX{initialX + distanceCoefficientX * stepsX}, distanceY{initialY + distanceCoefficientY * stepsY};
			if(totalStepsY == 0 || (distanceX <= distanceY && totalStepsX > 0))
			{
				startTile.first += directionX;
				stepsX++;
				totalStepsX--;
				if(distanceX >= minCollision.distance)
					return;

				else if(Game::getTileSolid(startTile.first, startTile.second))
				{
					minCollision.type = Collision::eVertical;
					minCollision.positionX = startTile.first - directionX + 0.5f;
					minCollision.positionY = startTile.second + 0.5f;
					minCollision.distance = distanceX;
					return;
				}
			}
			else
			{
				startTile.second += directionY;
				stepsY++;
				totalStepsY--;
				if(distanceY >= minCollision.distance)
					return;

				else if(Game::getTileSolid(startTile.first, startTile.second))
				{
					minCollision.type = Collision::eHorizontal;
					minCollision.positionX = startTile.first + 0.5f;
					minCollision.positionY = startTile.second - directionY + 0.5f;
					minCollision.distance = distanceY;
					return;
				}
			}
		}
	};

	Collision minCollision;
	calculateMinCollision(previousPositiveX,
						  previousNegativeY,
						  positiveX,
						  negativeY,
						  minCollision);
	calculateMinCollision(previousNegativeX,
						  previousPositiveY,
						  negativeX,
						  positiveY,
						  minCollision);
	calculateMinCollision(previousPositiveX,
						  previousPositiveY,
						  positiveX,
						  positiveY,
						  minCollision);

	if(minCollision.type == Collision::eVertical)
	{
		minCollision.type = Collision::eNone;
		minCollision.distance = std::numeric_limits<double>::max();
		velocityX = 0.0;
		x = minCollision.positionX * QuadData::tileScale.x;

		distanceCoefficientX = 100000.0;
		distanceCoefficientY = 1.0;
		calculateMinCollision((x + QuadData::tileScale.x * 0.49f * directionX) / QuadData::tileScale.x,
							  minCollision.positionY + 0.49f * directionY,
							  (x + QuadData::tileScale.x * 0.49f * directionX) / QuadData::tileScale.x,
							  positiveY,
							  minCollision);
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
		calculateMinCollision(minCollision.positionX + 0.49f * directionX,
							  (y + QuadData::tileScale.y * 0.49f * directionY) / QuadData::tileScale.y,
							  positiveX,
							  (y + QuadData::tileScale.y * 0.49f * directionY) / QuadData::tileScale.y,
							  minCollision);
		if(minCollision.type == Collision::eVertical)
		{
			velocityX = 0.0;
			x = minCollision.positionX * QuadData::tileScale.x;
		}
	}

	previousX = x;
	previousY = y;
}