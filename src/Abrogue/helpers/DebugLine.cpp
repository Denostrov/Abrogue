module DebugLine;

DebugLine::DebugLine(double startX, double startY, double endX, double endY)
{
	double distanceX = endX - startX;
	double distanceY = (endY - startY) * 2.0;
	double distance = std::sqrt(distanceX * distanceX + distanceY * distanceY);
	double cos = distanceX / distance;
	double sin = distanceY / distance;

	QuadData quadData{{startX + distanceX / 2.0, startY + distanceY / 4.0}, {Color::pack(255, 0, 0, 255), Color::pack(255, 0, 0, 255)}, 179};
	quadData.setRotation(-sin, cos);
	quadData.setScale(0.1, distance / 2.0);
	quad = quadPool.insert(quadData, QuadPool::eItem);
}