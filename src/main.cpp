#include <PixL_Renderer.hpp>
#include <iostream>
#include <thread>

int main(int argc, char *argv[])
{
	std::cout << "Hello, World!" << std::endl;

	// Initialize the renderer
	PixL_Renderer_Init(PIXL_RENDERER_PLAN_MODE | PIXL_RENDERER_PLAN_MODE_AUTO_CLEAR);

	// Get the window
	SDL_Window *window = CreateWindow("PixL Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);

	bool quit = false;
	while (!quit)
	{
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			if (e.type == SDL_QUIT)
			{
				quit = true;
			}
		}
	}

	return 0;
}