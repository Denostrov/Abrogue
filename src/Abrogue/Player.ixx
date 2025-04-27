export module Player;

export import PhysicsComponent;
export import QuadPool;

export struct Weapon
{
	Weapon() = default;

	std::uint64_t damage{};
	double attackTime{};
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
	void updateDraw(double deltaTime)
	{
		float guiOffset = 48.0f;

		auto [x, y] = getPosition();
		auto [vx, vy] = getVelocity();
		quadReference.setPosition({(guiOffset + x + vx * deltaTime) * QuadData::tileScale.x, (y + vy * deltaTime) * QuadData::tileScale.y});
		if(attackTimer > 0.0)
		{
			double attackPeak = weapon.attackTime / 2.0;
			double weaponOffset = (attackPeak - std::abs(attackTimer - attackPeak)) / attackPeak;
			weaponReference.setPosition({(guiOffset + x + vx * deltaTime + weaponOffset * attackAngleCos) * QuadData::tileScale.x, (y + vy * deltaTime + weaponOffset * attackAngleSin) * QuadData::tileScale.y});
		}
	}

	Weapon weapon;
	QuadPool::Reference weaponReference;
	double attackTimer{};
	double attackAngleCos{};
	double attackAngleSin{};

	QuadPool::Reference quadReference;
};