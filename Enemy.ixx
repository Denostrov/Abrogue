export module Enemy;

export import PhysicsComponent;

export class Enemy : public PhysicsComponent
{
public:
	Enemy();

	void update();
};