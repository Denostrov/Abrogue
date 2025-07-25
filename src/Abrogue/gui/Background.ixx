export module Background;

export import QuadPool;

//Class for a semi transparent rectangle that covers the whole screen
//The rectangle fades in over time after being shown
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