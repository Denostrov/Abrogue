export module PhysicsComponent;

export import std;
export import Constants;
export import ObjectPools;

export class PhysicsComponent
{
public:
	PhysicsComponent();

	std::pair<double, double> getPosition() const { return {x, y}; }

	void setMass(double newMass) { mass = newMass; }
	void setFrictionCoefficient(double newFriction) { frictionCoefficient = newFriction; }
	void setMaxSpeed(double newMaxSpeed) { maxSpeed = newMaxSpeed; }
	void setMovementX(std::int32_t direction) { movementDirectionX = direction; }
	void setMovementY(std::int32_t direction) { movementDirectionY = direction; }

	void update();

private:
	double x{}, y{};
	double velocityX{}, velocityY{};
	double mass{1.0};
	double frictionCoefficient{1.0};
	double maxSpeed{1.0};

	std::int32_t movementDirectionX{}, movementDirectionY{};

	QuadPool::Reference quadReference;
};