export module Player;

export import Constants;
export import PhysicsComponent;

export class Player : public PhysicsComponent
{
public:
	Player()
	{
		auto [x, y] = getPosition();
		quadReference = QuadPool::insert(QuadData{{0.0f, 0.0f}, QuadData::tileScale,
										 {QuadData::packColor(255, 255, 255, 255), QuadData::packColor(255, 255, 255, 255), 0}, 64});
	}

	void update()
	{
		PhysicsComponent::update();
	}

	QuadPool::Reference quadReference;
};