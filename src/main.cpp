#include <chrono>
#include <const/Config.hpp>
#include <Light.hpp>
#include <Log.hpp>
#include <OverworldRenderer.hpp>
#include <generated/map2cpp.hpp>
#include <generated/font2cpp.hpp>
#include <FontsRenderer.hpp>
#include <Cinematics.hpp>

using namespace std;
using namespace std::chrono;




int main(int argc, char* argv[])
{
	// init SDL
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		return 1;
	}

	// create window
	SDL_Window* window = SDL_CreateWindow("PixL Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if (!window)
	{
		return 1;
	}
	bool quit = false;

	// new camera struct
	camera* Camera = new camera();
	Camera->renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	Camera->position.x = 0;
	Camera->position.y = 0;
	Camera->size.x = WINDOW_WIDTH;
	Camera->size.y = WINDOW_HEIGHT;
	Camera->zoom = 1.0;
	Camera->logger = new Logger();
	Tileset* tileset = new Tileset(&Tileset_tileset1 , Camera);
	OverworldRenderer* overworld = new OverworldRenderer(Camera);
	FontsRenderer* font = new FontsRenderer(Camera, &font_ressource_m5x7);

	int counter = 0;

	// main loop
	while (!quit)
	{
		counter++;

		// debug effect
		//Camera->zoom = sin(counter / 5000.0f) + 2.0f;

		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			// quit
			if (event.type == SDL_QUIT)
			{
				quit = true;
			}

			// if window resized
			if (event.type == SDL_WINDOWEVENT)
			{
				if (event.window.event == SDL_WINDOWEVENT_RESIZED)
				{
					SDL_GetWindowSize(window, &Camera->size.x, &Camera->size.y);
				}
			}

			// if key pressed
			if (event.type == SDL_KEYDOWN)
			{
				switch (event.key.keysym.sym)
				{
					case SDLK_ESCAPE:
						quit = true;
						break;
					case SDLK_UP:
						Camera->position.y -= 32;
						break;
					case SDLK_DOWN:
						Camera->position.y += 32;
						break;
					case SDLK_LEFT:
						Camera->position.x -= 32;
						break;
					case SDLK_RIGHT:
						Camera->position.x += 32;
						break;
					case SDLK_KP_PLUS:
						Camera->zoom += 0.1f;
						break;
					case SDLK_KP_MINUS:
						Camera->zoom -= 0.01f;
						break;
				}
			}
		}

		// clear screen
		SDL_SetRenderDrawColor(Camera->renderer, 0, 0, 0, 255);
		SDL_RenderClear(Camera->renderer);

		// draw map
		overworld->check_maps_visibility();
		overworld->draw();
		
		uint8_t tr = static_cast<int>(255 * sin(0.01 * counter));
		uint8_t tg = static_cast<int>(255 * sin(0.01 * counter + 2));
		uint8_t tb = static_cast<int>(255 * sin(0.01 * counter + 4));
		// draw text
		font->render_text(100, 100, 64, 64, 32, u'R', {tr, tg, tb});
		font->render_text(132, 100, 64, 64, 32, u'G', {tr, tg, tb});
		font->render_text(164, 100, 64, 64, 32, u'B', {tr, tg, tb});

		// update screen
		SDL_RenderPresent(Camera->renderer);
		SDL_Delay(16);
	}


	return 0;
}