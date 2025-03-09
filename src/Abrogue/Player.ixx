export module Player;

export import PhysicsComponent;
export import QuadPool;

export class Player : public PhysicsComponent
{
public:
	Player() = default;
	Player(double velocity);

	void update()
	{
		PhysicsComponent::update();
	}

	QuadPool::Reference quadReference;
};