export module Enemy;

export import PhysicsComponent;
export import QuadPool;

export class Enemy : public PhysicsComponent
{
public:
	Enemy() = default;
	Enemy(std::uint8_t type);

	void update();

	QuadPool::Reference quadReference;
private:
	std::uint8_t type{};
};