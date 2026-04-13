module Abrogue:PhysicsComponent;

import :Constants;

/*
 * PhysicsComponent - class for handling movement and collisions
 */
class PhysicsComponent
{
public:
    PhysicsComponent() = default;
    PhysicsComponent(double x, double y, double leftScaleX, double rightScaleX, double topScaleY, double bottomScaleY);

    [[nodiscard]] std::pair<double, double> getPosition() const { return {x, y}; }
    [[nodiscard]] std::pair<double, double> getVelocity() const { return {velocityX, velocityY}; }
    [[nodiscard]] std::pair<double, double> getIntermediateVelocity() const { return {intermediateVelocityX, intermediateVelocityY}; }

protected:
    void setMass(double newMass) { mass = newMass; }
    void setFrictionCoefficient(double newFriction) { frictionCoefficient = newFriction; }
    void setMaxVelocity(double newMaxSpeed);
    void setMovementDirection(double directionX, double directionY);

    void update();

private:
    double x{}, y{};                                          //Coordinates in [0.0, mapWidth]x[0.0, mapHeight] space
    double velocityX{}, velocityY{};                          //Velocity in horizontal tiles per second
    double mass{1.0};                                         //Measures resistance to acceleration
    double frictionCoefficient{1.0};                          //Controls effectiveness of acceleration and deceleration
    double maxVelocity{1.0};                                  //Speed when in equilibrium with friction
    double resistanceCoefficient{20.0};                       //Controls effectiveness of deceleration due to velocity
    double walkingForce{maxVelocity * resistanceCoefficient}; //Force necessary to achieve max velocity

    double intermediateX{}, intermediateY{};
    double intermediateVelocityX{}, intermediateVelocityY{};

    double leftScaleX{0.48}, rightScaleX{0.48}; //Horizontal hitbox extents
    double topScaleY{0.48}, bottomScaleY{0.48}; //Vertical hitbox extents

    double previousX{x}, previousY{y}; //Coordinates on last update frame

    double movementDirectionX{}, movementDirectionY{}; //Unscaled directions of applied movement forces
};