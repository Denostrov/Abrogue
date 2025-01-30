module;

#include <SDL3/SDL_video.h>
#include <SDL3/SDL_vulkan.h>
#include <SDL3/SDL_messagebox.h>

#include <vulkan/vulkan.hpp>

export module RenderEngine;

export import std;

export class RenderEngine
{
public:
	void init(SDL_Window* newWindow)
	{
		window = newWindow;
		uint32_t instanceCount{};
		auto extensions = SDL_Vulkan_GetInstanceExtensions(&instanceCount);

		if(!extensions)
		{
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "SDL Error", SDL_GetError(), nullptr);
		}

		std::println(std::cerr, "{} vulkan instance extensions required:", instanceCount);
		for(uint32_t i{}; i < instanceCount; i++)
			std::println(std::cerr, "\t{}", extensions[i]);
	}

	void release() { window = nullptr; }

	auto getWindow() const { return window; }

private:
	SDL_Window* window{};
	vk::Instance vulkanInstance;
};

export inline RenderEngine renderEngine;