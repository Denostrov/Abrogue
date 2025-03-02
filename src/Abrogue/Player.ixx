export module Player;

export import Constants;
export import PhysicsComponent;

export class Player : public PhysicsComponent
{
public:
	Player()
	{
		auto [x, y] = getPosition();
		quadReference = QuadPool::insert(QuadData{{0.0f, 0.0f}, 1.0f,
										 {QuadData::packColor(64, 255, 0, 255), QuadData::packColor(64, 255, 0, 0)}, 64});

		setMaxSpeed(0.5);
	}

	void update()
	{
		PhysicsComponent::update();
	}

	QuadPool::Reference quadReference;
};