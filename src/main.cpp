#include <chrono>
#include <const/Config.hpp>
#include <Light.hpp>
#include <Log.hpp>
#include <Tileset.hpp>
#include <generated/tileset2cpp.hpp>
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



	// main loop
	while (!quit)
	{
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
		}

		// clear screen
		SDL_SetRenderDrawColor(Camera->renderer, 0, 0, 0, 255);
		SDL_RenderClear(Camera->renderer);
		for(int i = 0; i < tileset->get_tileset_height(); i++)
		{
			for(int j = 0; j < tileset->get_tileset_width(); j++)
			{
				tileset->draw_tile(i*tileset->get_tileset_width() + j  , j * TILE_SIZE, i * TILE_SIZE, TILE_SIZE, TILE_SIZE);
			}
		}

		// update screen
		SDL_RenderPresent(Camera->renderer);
	}


	return 0;
}