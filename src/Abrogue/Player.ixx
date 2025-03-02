export module Player;

export import Constants;
export import PhysicsComponent;

export class Player : public PhysicsComponent
{
public:
	Player();

	void update()
	{
		PhysicsComponent::update();
	}

	QuadPool::Reference quadReference;
};