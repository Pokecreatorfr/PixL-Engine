#include <PixL_Renderer.hpp>
#include <iostream>
#include <thread>

int main(int argc, char *argv[])
{
	// Initialize the renderer
	PixL_Renderer_Init(PIXL_RENDERER_PLAN_MODE | PIXL_RENDERER_PLAN_MODE_AUTO_CLEAR);
	// Get the window
	SDL_Window *window = CreateWindow("PixL Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
	// create a texture
	PixL_Texture *texture = CreateTexture("test.png");

	PixL_Draw_Property property;

	// Add the texture to the renderer
	int id = PixL_AddDrawable(texture, property);

	PixL_Draw_Property *prop = PixL_GetDrawableProperty(id);

	bool quit = false;
	while (!quit)
	{

		std::this_thread::sleep_for(std::chrono::milliseconds(1000 / 60));

		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			if (e.type == SDL_QUIT)
			{
				quit = true;
			}

			if (e.type == SDL_KEYDOWN)
			{
				if (e.key.keysym.sym == SDLK_UP)
				{
					if (prop->mosaic_mode > 0)
					{
						prop->mosaic_mode -= 1;
					}
					std::cout << "Mosaic mode :" << prop->mosaic_mode << std::endl;
				}
				if (e.key.keysym.sym == SDLK_DOWN)
				{
					if (prop->mosaic_mode < 255)
					{
						prop->mosaic_mode += 1;
					}
					std::cout << "Mosaic mode :" << prop->mosaic_mode << std::endl;
				}

			}
		}
		// std::cout << "1" << std::endl;
		PixL_Draw();
		PixL_Present();
	}

	return 0;
}