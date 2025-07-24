export module Background;

export import QuadPool;

//Class for semi transparent rectangle that covers the whole screen
export class Background
{
public:
	Background() = default;

	void updateDraw(double deltaTime);
	void setVisible(bool visible);

private:
	static constexpr double animationEndTime{2.0};

	double animationTime{};

	QuadPool::Reference quad;
};