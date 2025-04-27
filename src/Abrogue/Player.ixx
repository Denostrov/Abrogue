export module Player;

export import PhysicsComponent;
export import QuadPool;

export struct Weapon
{
	Weapon() = default;

	std::uint64_t damage{};
};

export class Player : public PhysicsComponent
{
public:
	Player() = default;
	Player(double velocity);

	void onMousePressed(std::uint32_t x, std::uint32_t y);

	void update()
	{
		PhysicsComponent::update();

		if(attackTimer > 0.0)
		{
			attackTimer -= Constants::tickDuration;

			if(attackTimer <= 0.0)
			{
				attackTimer = 0.0;
				weaponReference = QuadPool::Reference{};
			}
		}
	}

	Weapon weapon;
	QuadPool::Reference weaponReference;
	double attackTimer{};

	QuadPool::Reference quadReference;
};