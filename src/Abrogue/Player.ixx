export module Player;

export import Constants;
export import PhysicsComponent;

export class Player : public PhysicsComponent
{
public:
	Player() = default;
	Player(double speed);

	void update()
	{
		PhysicsComponent::update();
	}

	QuadPool::Reference quadReference;
};