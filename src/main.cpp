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
	property.x = 0.5;
	property.y = 0.5;
	property.w = 0.5;
	property.h = 0.5;
	property._plan = 1;

	// Add the texture to the renderer
	int id = PixL_AddDrawable(texture, property);

	property.r = 150;
	property.a = 200;
	int id1 = PixL_AddDrawable(texture, property);

	float x = 0.25;
	float y = 0.5;
	bool up = true;
	bool right = true;

	float x1 = 0.75;
	float y1 = 0.5;
	bool up1 = true;
	bool right1 = false;

	PixL_Draw_Property *prop = PixL_GetDrawableProperty(id);
	PixL_Draw_Property *prop1 = PixL_GetDrawableProperty(id1);

	bool quit = false;
	while (!quit)
	{
		if (up)
		{
			y -= 0.001;
		}
		else
		{
			y += 0.001;
		}

		if (right)
		{
			x += 0.004;
		}
		else
		{
			x -= 0.004;
		}

		if (y <= 0.125)
		{
			up = false;
		}
		else if (y >= 0.875)
		{
			up = true;
		}

		if (x <= 0.125)
		{
			right = true;
		}
		else if (x >= 0.875)
		{
			right = false;
		}

		if (up1)
		{
			y1 -= 0.006;
		}
		else
		{
			y1 += 0.006;
		}

		if (right1)
		{
			x1 += 0.004;
		}
		else
		{
			x1 -= 0.004;
		}

		if (y1 <= 0.125)
		{
			up1 = false;
		}
		else if (y1 >= 0.875)
		{
			up1 = true;
		}

		if (x1 <= 0.125)
		{
			right1 = true;
		}
		else if (x1 >= 0.875)
		{
			right1 = false;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(1000 / 60));

		prop->x = x;
		prop->y = y;
		prop1->x = x1;
		prop1->y = y1;

		// prop1->_plan = rand() % 3;
		prop1->rot += 1;

		// prop->_plan = rand() % 3;
		prop->rot -= 1;

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