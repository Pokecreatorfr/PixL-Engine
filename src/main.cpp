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
	// create a texture
	PixL_Texture *texture = CreateTexture("test.png");

	PixL_Draw_Property property;
	property.x = 0.5;
	property.y = 0.5;
	property.w = 0.5;
	property.h = 0.5;
	property.a = 0;
	property.r = 0;
	property._plan = 1;

	// Add the texture to the renderer
	int id = PixL_AddDrawable(texture, property);

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
		// std::cout << "1" << std::endl;
		PixL_Draw();
		PixL_Present();
	}

	return 0;
}