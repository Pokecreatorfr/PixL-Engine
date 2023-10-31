#include <chrono>
#include <Render.hpp>
#include <maptocpp.hpp>
#include <imagetocpp.hpp>
#include <const/Config.hpp>

using namespace std;
using namespace std::chrono;




int main(int argc, char* argv[])
{
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		cout << "SDL initialization failed. SDL Error: " << SDL_GetError();
	}
	else
	{
		cout << "SDL initialization succeeded!" << endl;
	}


	SDL_Window* window = SDL_CreateWindow(	"2D_ENGINE",
											SDL_WINDOWPOS_CENTERED,
											SDL_WINDOWPOS_CENTERED,
											WINDOW_WIDTH, WINDOW_HEIGHT,
											SDL_WINDOW_RESIZABLE);
	if (!window)
	{
		std::cout << "Failed to create window\n";
		return -1;
	}

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	bool running = true;


	auto millisec_since_epoch = chrono::duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

	int frames_since_start = 0;

	Tileset* tileset = Load_Tileset(tileset1_ressource, renderer, 32, 32);
	if (!tileset)
	{
		cout << "Failed to load tileset\n";
		return -1;
	}

	Camera camera = { {0,0}, renderer , {0,0}, 1 };

	World_Renderer *world_renderer = new World_Renderer(&world_world, &camera);
	camera.Get_View_Size(window);
	world_renderer->load_weather(0);
	Logger logger;
	
	bool weather_loaded = true;

	while (running)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT) running = false;

			// if arrow keys are pressed, move camera
			if (event.type == SDL_KEYDOWN)
			{
				if (event.key.keysym.sym == SDLK_UP)
				{
					camera.position.y -= 16;
				}
				if (event.key.keysym.sym == SDLK_DOWN)
				{
					camera.position.y += 16;
				}
				if (event.key.keysym.sym == SDLK_LEFT)
				{
					camera.position.x -= 16;
				}
				if (event.key.keysym.sym == SDLK_RIGHT)
				{
					camera.position.x += 16;
				}
				// if + or - is pressed, zoom in or out
				if (event.key.keysym.sym == SDLK_KP_MINUS)
				{
					if (camera.zoom > 0.5f)
						camera.zoom -= 0.5f;
					logger.Log("camera zoom : " + to_string(camera.zoom));
				}
				if (event.key.keysym.sym == SDLK_KP_PLUS)
				{
					camera.zoom += 0.5f;
					logger.Log("camera zoom : " + to_string(camera.zoom));
				}
				// if key N is pressed, change weather
				if (event.key.keysym.sym == SDLK_n)
				{
					weather_loaded == true ? world_renderer->unload_weather() : world_renderer->load_weather(0);
					weather_loaded = !weather_loaded;
				}
			}
			
		}
		camera.Get_View_Size(window);
		world_renderer->update();
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);



		world_renderer->Draw(tileset);


		SDL_RenderPresent(renderer);
		while (chrono::duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count() - millisec_since_epoch < 1000 / FPS);
		//cout<<chrono::duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count() - millisec_since_epoch<<endl;
		millisec_since_epoch = chrono::duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
		frames_since_start++;
		//cout << "camera position: " << camera.position.x << ", " << camera.position.y << endl;
	}
	SDL_Quit();
	logger.Log("frames since start : " + to_string(frames_since_start));
	logger.~Logger();
	return 0;
}