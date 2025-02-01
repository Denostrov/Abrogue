export module Game;

export import RenderEngine;

export class Game
{
public:
	static bool init()
	{
		renderEngine = std::make_unique<RenderEngine>();
		if(renderEngine->getHasError())
			return false;

		return true;
	}
	static void release()
	{
		renderEngine.reset();
	}

private:
	inline static std::unique_ptr<RenderEngine> renderEngine;
};