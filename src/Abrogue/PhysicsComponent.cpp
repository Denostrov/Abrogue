module PhysicsComponent;

PhysicsComponent::PhysicsComponent()
{
	quadReference = QuadPool::insert(QuadData{{x, y}, {0.02f, 0.04f}});
}

void PhysicsComponent::update()
{
	double frictionCoefficient{1.0};
	double resistanceCoefficient{20.0};
	double walkingForce{maxSpeed * resistanceCoefficient};

	auto calculateVelocityAfterFriction = [this, frictionCoefficient, resistanceCoefficient](double velocity)
	{
		if(std::abs(velocity) > 0.0)
		{
			auto velocitySign = std::signbit(velocity);
			auto slowSpeed = std::max(0.2 * maxSpeed, std::abs(velocity));
			velocity -= std::copysign(slowSpeed, velocity) * frictionCoefficient * resistanceCoefficient / mass * Constants::tickDuration;
			if(std::signbit(velocity) != velocitySign)
				velocity = 0.0;
		}

		return velocity;
	};

	x += velocityX * Constants::tickDuration;
	y += velocityY * Constants::tickDuration;

	velocityX = calculateVelocityAfterFriction(velocityX);
	velocityY = calculateVelocityAfterFriction(velocityY);

	double forceX = movementDirectionX * walkingForce * (movementDirectionX != 0 ? 1.0 / std::sqrt(2.0) : 1.0);
	double forceY = movementDirectionY * walkingForce * (movementDirectionY != 0 ? 1.0 / std::sqrt(2.0) : 1.0);

	velocityX += forceX / mass * frictionCoefficient * Constants::tickDuration;
	velocityY += forceY / mass * frictionCoefficient * Constants::tickDuration;

	quadReference.set(QuadData{{x, y}, {0.02f, 0.04f}});
}