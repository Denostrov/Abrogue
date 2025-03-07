module PhysicsComponent;

import Game;

PhysicsComponent::PhysicsComponent(double x, double y, double leftScaleX, double rightScaleX, double topScaleY, double bottomScaleY)
	:x(x), y(y), leftScaleX(leftScaleX), rightScaleX(rightScaleX), topScaleY(topScaleY), bottomScaleY(bottomScaleY)
{}

void PhysicsComponent::update()
{
	auto calculateNextStep = [this](double coordinate, double velocity, double tileScale, int32_t movementDirection, int32_t otherMovementDirection)
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
		result.second += (movementForce * tileScale + frictionForce) / mass * Constants::tickDuration / 2.0;
		if(std::signbit(result.second) != velocitySign && movementForce == 0.0)
			result.second = 0.0;

		return result;
	};

	//Do forward euler integration in two steps to reflect coordinate change on same tick
	std::tie(x, velocityX) = calculateNextStep(x, velocityX, 1.0, movementDirectionX, movementDirectionY);
	std::tie(y, velocityY) = calculateNextStep(y, velocityY, Constants::tileAspectRatio, movementDirectionY, movementDirectionX);

	std::tie(x, velocityX) = calculateNextStep(x, velocityX, 1.0, movementDirectionX, movementDirectionY);
	std::tie(y, velocityY) = calculateNextStep(y, velocityY, Constants::tileAspectRatio, movementDirectionY, movementDirectionX);

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
		} type{};	//Type of wall that was hit
		double positionX{}, positionY{};	//Position of tile center during collision
		double distance{std::numeric_limits<double>::max()}; //Distance to collision point
	};

	std::int32_t directionX = x >= previousX ? 1 : -1;
	std::int32_t directionY = y >= previousY ? 1 : -1;

	double positiveOffsetX = directionX == 1 ? rightScaleX : -leftScaleX;
	double negativeOffsetX = directionX == 1 ? -leftScaleX : rightScaleX;
	double positiveOffsetY = directionY == 1 ? bottomScaleY : -topScaleY;
	double negativeOffsetY = directionY == 1 ? -topScaleY : bottomScaleY;

	double previousPositiveX = previousX + positiveOffsetX;
	double positiveX = x + positiveOffsetX;

	double previousNegativeX = previousX + negativeOffsetX;
	double negativeX = x + negativeOffsetX;

	double previousPositiveY = previousY + positiveOffsetY;
	double positiveY = y + positiveOffsetY;

	double previousNegativeY = previousY + negativeOffsetY;
	double negativeY = y + negativeOffsetY;

	auto calculateMinCollision = [this, directionX, directionY](double startX, double startY, double endX, double endY,
																double distanceCoefficientX, double distanceCoefficientY,
																double offsetX, double offsetY,
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
					minCollision.positionX = startTile.first + (directionX == -1) - offsetX * 1.001;
					minCollision.positionY = startTile.second + 0.5;
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
					minCollision.positionX = startTile.first + 0.5;
					minCollision.positionY = startTile.second + (directionY == -1) - offsetY * 1.001;
					minCollision.distance = distanceY;
					return;
				}
			}
		}
	};

	Collision minCollision;
	calculateMinCollision(previousPositiveX, previousNegativeY,
						  positiveX, negativeY,
						  distanceCoefficientX, distanceCoefficientY,
						  positiveOffsetX, negativeOffsetY,
						  minCollision);
	calculateMinCollision(previousNegativeX, previousPositiveY,
						  negativeX, positiveY,
						  distanceCoefficientX, distanceCoefficientY,
						  negativeOffsetX, positiveOffsetY,
						  minCollision);
	calculateMinCollision(previousPositiveX, previousPositiveY,
						  positiveX, positiveY,
						  distanceCoefficientX, distanceCoefficientY,
						  positiveOffsetX, positiveOffsetY,
						  minCollision);

	if(minCollision.type == Collision::eVertical)
	{
		minCollision.type = Collision::eNone;
		minCollision.distance = std::numeric_limits<double>::max();

		velocityX = 0.0;
		x = minCollision.positionX;

		calculateMinCollision(x + positiveOffsetX, minCollision.positionY + positiveOffsetY,
							  x + positiveOffsetX, positiveY,
							  100000.0, 1.0,
							  positiveOffsetX, positiveOffsetY,
							  minCollision);
		if(minCollision.type == Collision::eHorizontal)
		{
			velocityY = 0.0;
			y = minCollision.positionY;
		}
	}
	else if(minCollision.type == Collision::eHorizontal)
	{
		minCollision.type = Collision::eNone;
		minCollision.distance = std::numeric_limits<double>::max();

		velocityY = 0.0;
		y = minCollision.positionY;

		calculateMinCollision(minCollision.positionX + positiveOffsetX, y + positiveOffsetY,
							  positiveX, y + positiveOffsetY,
							  1.0, 100000.0,
							  positiveOffsetX, positiveOffsetY,
							  minCollision);
		if(minCollision.type == Collision::eVertical)
		{
			velocityX = 0.0;
			x = minCollision.positionX;
		}
	}

	previousX = x;
	previousY = y;
}