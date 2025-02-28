export module Enemy;

export import PhysicsComponent;

export class Enemy : public PhysicsComponent
{
public:
	Enemy();

	void update();

private:
	std::uint32_t type{};
	QuadPool::Reference quadReference;
};