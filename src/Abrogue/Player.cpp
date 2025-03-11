module Player;

import GameSystems;

Player::Player(double velocity) :PhysicsComponent(36.0, 18.0, 0.48, 0.48, 0.32, 0.4)
{
	quadReference = quadPool.insert(QuadData{{0.0f, 0.0f}, 1.0f,
									 {Helpers::packColor(64, 255, 0, 255), Helpers::packColor(64, 255, 0, 0)}, 64});

	setMaxVelocity(velocity);
}