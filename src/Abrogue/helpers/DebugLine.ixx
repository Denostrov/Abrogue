export module DebugLine;

export import QuadPool;

export class DebugLine
{
public:
	DebugLine() = default;
	DebugLine(double startX, double startY, double endX, double endY);

private:
	QuadPool::Reference quad;
};