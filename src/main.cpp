#include <chrono>
#include <const/Config.hpp>
#include <SDL2/SDL.h>
#include <Light.hpp>
#include <Log.hpp>
#include <iostream>
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
	SDL_Window* window = SDL_CreateWindow("PixL Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if (!window)
	{
		cout << "Failed to create window\n";
		return 1;
	}
	bool quit = false;

	Logger* logger = new Logger();
	logger->log("logger created");
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
	}


	return 0;
}