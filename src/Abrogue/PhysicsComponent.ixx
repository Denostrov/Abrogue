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
	void setMaxVelocity(double newMaxSpeed) { maxVelocity = newMaxSpeed; walkingForce = maxVelocity * resistanceCoefficient; }
	void setMovementX(std::int32_t direction) { movementDirectionX = direction; }
	void setMovementY(std::int32_t direction) { movementDirectionY = direction; }

	void update();

private:
	double x{}, y{};					//Coordinates in [0.0, mapWidth]x[0.0, mapHeight] space
	double velocityX{}, velocityY{};	//Velocity in horizontal tiles per second
	double mass{1.0};					//Measures resistance to acceleration
	double frictionCoefficient{1.0};	//Controls effectiveness of acceleration and deceleration
	double maxVelocity{1.0};			//Speed when in equilibrium with friction
	double resistanceCoefficient{20.0};	//Controls effectiveness of deceleration due to velocity
	double walkingForce{maxVelocity * resistanceCoefficient};	//Force necessary to achieve max velocity

	double leftScaleX{0.48}, rightScaleX{0.48};	//Horizontal hitbox extents
	double topScaleY{0.48}, bottomScaleY{0.48};	//Vertical hitbox extents

	double previousX{x}, previousY{y};	//Coordinates on last update frame

	std::int32_t movementDirectionX{}, movementDirectionY{};	//Direction of applied movement forces (0 - no force, 1 - positive, -1 - negative)
};