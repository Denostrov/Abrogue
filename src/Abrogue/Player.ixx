export module Player;

export import Constants;
export import PhysicsComponent;

export class Player : public PhysicsComponent
{
public:
	Player()
	{
		auto [x, y] = getPosition();
		quadReference = QuadPool::insert(QuadData{{x, y}, {0.16f, 0.32f}, {1.0f, 1.0f, 1.0f}, 64.0f});
	}

	void update()
	{
		PhysicsComponent::update();
		auto [x, y] = getPosition();
		quadReference.set(QuadData{{x, y}, {0.16f, 0.32f}, {1.0f, 1.0f, 1.0f}, 64.0f});
	}

private:
	QuadPool::Reference quadReference;
};