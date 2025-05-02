export module Player;

export import PhysicsComponent;
export import QuadPool;
export import Weapon;

export class Player : public PhysicsComponent
{
public:
	Player() = default;
	Player(double velocity);

	void onMousePressed(std::uint32_t x, std::uint32_t y);

	void update();
	void updateDraw(double deltaTime);

	Weapon weapon;

	QuadPool::Reference quadReference;
};