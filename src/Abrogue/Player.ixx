export module Player;

export import Constants;
export import PhysicsComponent;

export class Player : public PhysicsComponent
{
public:
	void update()
	{
		PhysicsComponent::update();
	}
};