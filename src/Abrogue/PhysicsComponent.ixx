export module PhysicsComponent;

export import std;
export import Constants;
export import ObjectPools;

export class PhysicsComponent
{
public:
	PhysicsComponent() = default;
	PhysicsComponent(double x, double y, double leftScaleX, double rightScaleX, double topScaleY, double bottomScaleY);

	std::pair<double, double> getPosition() const { return {x, y}; }
	std::pair<double, double> getVelocity() const { return {velocityX, velocityY}; }

	void setMass(double newMass) { mass = newMass; }
	void setFrictionCoefficient(double newFriction) { frictionCoefficient = newFriction; }
	void setMaxSpeed(double newMaxSpeed) { maxSpeed = newMaxSpeed; walkingForce = maxSpeed * resistanceCoefficient; }
	void setMovementX(std::int32_t direction) { movementDirectionX = direction; }
	void setMovementY(std::int32_t direction) { movementDirectionY = direction; }

	void update();

private:
	double x{}, y{};
	double velocityX{}, velocityY{};
	double mass{1.0};
	double frictionCoefficient{1.0};
	double maxSpeed{1.0};
	double resistanceCoefficient{20.0};
	double walkingForce{maxSpeed * resistanceCoefficient};

	double leftScaleX{0.48}, rightScaleX{0.48};
	double topScaleY{0.48}, bottomScaleY{0.48};

	double previousX{x}, previousY{y};

	std::int32_t movementDirectionX{}, movementDirectionY{};
};