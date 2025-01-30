module;

#include <vulkan/vulkan.hpp>

export module RenderEngine;

export import std;
export import RenderWindow;

export class RenderEngine
{
public:
	bool init()
	{
		window = std::make_unique<RenderWindow>();
		if(window->getHasError())
			return false;

		return true;
	}

	void release() { window.release(); }

	auto& getWindow() const { return window; }

private:
	std::unique_ptr<RenderWindow> window;
	vk::Instance vulkanInstance;
};

export inline RenderEngine renderEngine;