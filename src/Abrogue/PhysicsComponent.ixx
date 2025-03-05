export module PhysicsComponent;

export import std;
export import Constants;
export import ObjectPools;

export class PhysicsComponent
{
public:
	std::pair<double, double> getPosition() const { return {x, y}; }
	std::pair<double, double> getVelocity() const { return {velocityX, velocityY}; }

	void setMass(double newMass) { mass = newMass; }
	void setFrictionCoefficient(double newFriction) { frictionCoefficient = newFriction; }
	void setMaxSpeed(double newMaxSpeed) { maxSpeed = newMaxSpeed; walkingForce = maxSpeed * resistanceCoefficient; }
	void setMovementX(std::int32_t direction) { movementDirectionX = direction; }
	void setMovementY(std::int32_t direction) { movementDirectionY = direction; }

	void update();

private:
	double x{0.5}, y{0.5};
	double velocityX{}, velocityY{};
	double mass{1.0};
	double frictionCoefficient{1.0};
	double maxSpeed{1.0};
	double resistanceCoefficient{20.0};
	double walkingForce{maxSpeed * resistanceCoefficient};

	bool isEnteringCorridor{};
	float corridorX{}, corridorY{};
	bool isInHorizontalCorridor{};
	bool isInVerticalCorridor{};

	std::int32_t movementDirectionX{}, movementDirectionY{};
};