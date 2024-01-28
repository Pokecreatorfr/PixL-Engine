#include <chrono>
#include <const/Config.hpp>
#include <SDL2/SDL.h>
#include <Light.hpp>
#include <generated/map2cpp.hpp>
#include <Renderer.hpp>
using namespace std;
using namespace std::chrono;




int main(int argc, char* argv[])
{
	// init SDL
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		cout << "Failed to init SDL\n";
		return 1;
	}

	// create window
	SDL_Window* window = SDL_CreateWindow("PixL Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
	if (!window)
	{
		cout << "Failed to create window\n";
		return 1;
	}

	// create renderer
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (!renderer)
	{
		cout << "Failed to create renderer\n";
		return 1;
	}

	SDL_Texture *texture = Generate_Light_Texture(renderer, 100, 255, 255, 0, 100);
	SDL_Texture *texture2 = Generate_Light_Texture(renderer, 100, 255, 255, 0, 100);
	SDL_SetTextureColorMod(texture2, 200, 0, 200);

	bool quit = false;
	
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
		}

		// clear screen
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);
		
		// draw
		SDL_Rect rect;
		rect.x = 0;
		rect.y = 0;
		rect.w = 100;
		rect.h = 100;
		SDL_RenderCopy(renderer, texture, NULL, &rect);
		rect.x = 50;
		rect.y = 50;
		SDL_RenderCopy(renderer, texture, NULL, &rect);
		// update screen
		SDL_RenderPresent(renderer);
	}


	return 0;
}