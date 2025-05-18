export module Background;

export import QuadPool;

//Class for fading background behind popup menus
export class Background
{
public:
	Background() = default;

	void updateDraw(double deltaTime);
	void setVisible(bool visible);

private:
	static constexpr double fadeTime{2.0};	//Time until end of animation
	double animationTime{};					//Current animation time

	QuadPool::Reference quad;	//Semi transparent rectangle that covers the whole screen
};