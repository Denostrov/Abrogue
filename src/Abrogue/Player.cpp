module Player;

Player::Player(double speed)
{
	quadReference = QuadPool::insert(QuadData{{0.0f, 0.0f}, 1.0f,
									 {QuadData::packColor(64, 255, 0, 255), QuadData::packColor(64, 255, 0, 0)}, 64});

	setMaxSpeed(speed);
}