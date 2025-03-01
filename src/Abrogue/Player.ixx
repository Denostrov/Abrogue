export module Player;

export import Constants;
export import PhysicsComponent;

export class Player : public PhysicsComponent
{
public:
	Player()
	{
		auto [x, y] = getPosition();
		quadReference = QuadPool::insert(QuadData{{x, y}, {0.01666f, 0.03333f},
										 {QuadData::packColor(255, 255, 255, 255), QuadData::packColor(255, 255, 255, 255), 0}, 64});
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