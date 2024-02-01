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
	bool quit = false;

	overworld_vars overworld_struct;
	overworld_struct.camera.position.x = 0;
	overworld_struct.camera.position.y = 0;
	overworld_struct.camera.zoom = 1;
	overworld_struct.camera.size.x = WINDOW_WIDTH;
	overworld_struct.camera.size.y = WINDOW_HEIGHT;

	Logger* logger = new Logger();
	logger->log("logger created");

	Renderer* renderer_class = new Renderer(window, &overworld_struct, logger);
	Overworld* overworld = renderer_class->GetOverworld();
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
			// if window is resized
			if (event.type == SDL_WINDOWEVENT)
			{
				if (event.window.event == SDL_WINDOWEVENT_RESIZED)
				{
					// update window size
					SDL_GetWindowSize(window, &overworld_struct.camera.size.x, &overworld_struct.camera.size.y);
				}
			}
			// if arrow keys are pressed
			if (event.type == SDL_KEYDOWN)
			{
				switch (event.key.keysym.sym)
				{
				case SDLK_UP:
					overworld_struct.camera.position.y -= 10;
					break;
				case SDLK_DOWN:
					overworld_struct.camera.position.x += 10;
					break;
				case SDLK_LEFT:
					overworld_struct.camera.position.x -= 10;
					break;
				case SDLK_RIGHT:
					overworld_struct.camera.position.y += 10;
					break;
				case SDLK_KP_PLUS:
					overworld_struct.camera.zoom += 0.1;
					break;
				case SDLK_KP_MINUS:
					overworld_struct.camera.zoom -= 0.1;
					break;
				}
			}
		}
		overworld->update();
		overworld->draw();
		renderer_class->draw();
	}


	return 0;
}