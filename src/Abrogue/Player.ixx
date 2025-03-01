export module Player;

export import Constants;
export import PhysicsComponent;

export class Player : public PhysicsComponent
{
public:
	Player()
	{
		auto [x, y] = getPosition();
		quadReference = QuadPool::insert(QuadData{{x, y}, {0.16f, 0.32f},
										 {QuadData::packColor(255, 255, 255, 255), QuadData::packColor(0, 255, 0, 255), 0}, 64});
	}

	void update()
	{
		PhysicsComponent::update();
		auto [x, y] = getPosition();
		quadReference.setPosition({x, y});
	}

private:
	QuadPool::Reference quadReference;
};