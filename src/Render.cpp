#include <Render.hpp>




Tilemap_Renderer::Tilemap_Renderer(Map* map, int layer)
{
	for (int i = 0; i < map->height; i++)
	{
		for (int j = 0; j < map->width; j++)
		{
			int tile_index = map->tilemaps[layer][i * map->width + j];
			if (tile_index != 0)
			{
				coord_2d position;
				position.x = (j + map->x ) * 32;
				position.y = (i + map->y ) * 32;
				coord_2d size;
				size.x = 32;
				size.y = 32;
				Tile tile(position, size, tile_index);
				tiles.push_back(tile);
			}
		}
	}
}


void Tilemap_Renderer::Draw(Tileset* tileset, Camera camera)
{
	for (int i = 0; i < tiles.size(); i++)
	{
		SDL_Rect rect;
		// render tiles with camera offset and zoom factor ; code foru
		rect.x = (tiles[i].position.x - camera.position.x) * camera.zoom + camera.size.x / 2;
		rect.y = (tiles[i].position.y - camera.position.y) * camera.zoom + camera.size.y / 2;
		rect.w = (tiles[i].size.x)* camera.zoom;
		rect.h = (tiles[i].size.y)* camera.zoom;
		SDL_RenderCopy(camera.renderer, tileset->texture, &tileset->tiles[tiles[i].tile_index-1], &rect);
	}
}

World_Renderer::World_Renderer(World* world, Camera* cam)
{
	camera = cam;
	this->world = world;
}

World_Renderer::~World_Renderer()
{
	for (int i = 0; i < (sizeof(Map_Renderers) / sizeof(Map_Renderer*)); i++)
	{
		if (Map_Renderers[i] != nullptr)
		{
			Map_Renderers[i]->~Map_Renderer();
		}
	}
}

void World_Renderer::Draw(Tileset* tileset)
{
	for (int i = 0; i < (sizeof(Map_Renderers) / sizeof(Map_Renderer*)); i++)
	{
		if (Map_Renderers[i] != nullptr)
		{
			Map_Renderers[i]->Draw(tileset);
		}
	}
	if (weather != nullptr)
	{
		weather->Draw();
	}
}

void World_Renderer::update()
{
	// check if loaded Map_Renderer is still in camera view
	// if not, unload it
	for(int i=0 ; i< (sizeof(Map_Renderers)/sizeof(Map_Renderer*)) ; i++)
	{
		if (Map_Renderers[i] != nullptr && !check_visibility(Map_Renderers[i]->get_position(), Map_Renderers[i]->get_size(), *camera))
		{
			Map_Renderers[i]->~Map_Renderer();
			Map_Renderers[i] = nullptr;
			cout << "Map_Renderer " << i <<" unloaded" << endl;

		}
	}

	// check if Map_Renderer needs to be loaded
	// if yes, load it
	for (int i = 0; i < world->maps.size(); i++)
	{
		if (check_visibility({ world->maps[i].x, world->maps[i].y }, { world->maps[i].width, world->maps[i].height }, *camera) == true)
		{
			//check if Map_Renderer is already loaded
			bool loaded = false;
			for (int j = 0; j < (sizeof(Map_Renderers) / sizeof(Map_Renderer*)); j++)
			{
				if (Map_Renderers[j] != nullptr && Map_Renderers[j]->get_position().x == world->maps[i].x && Map_Renderers[j]->get_position().y == world->maps[i].y)
				{
					loaded = true;
					break;
				}
			}
			if (loaded == false)
			{ 
				// check if there is an empty slot in Map_Renderers
				for (int j = 0; j < (sizeof(Map_Renderers) / sizeof(Map_Renderer*)); j++)
				{
					if (Map_Renderers[j] == nullptr)
					{
						Map_Renderers[j] = new Map_Renderer(&world->maps[i], camera);
						cout << "Map " << i << " loaded" << endl;
						break;

						// if no empty slot
						if ( j == (sizeof(Map_Renderers) / sizeof(Map_Renderer*)) - 1)
						{
							cout << "No empty slot in Map_Renderers" << endl;
						}
					}
				}
			}
			
		}
	}

	// update weather
	if (weather != nullptr)
	{
		weather->Update();
	}
}

void World_Renderer::load_tileset(const char* file_path, SDL_Renderer* renderer, int tile_width, int tile_height)
{
}

void World_Renderer::load_weather(int weather_index)
{
	weather = new Weather(camera, weather_index);
}

void World_Renderer::unload_weather()
{
	weather->~Weather();
	weather = nullptr;
}

bool check_visibility(coord_2d position, coord_2d size, Camera camera)
{
	float camsx = camera.size.x / 2;
	float camsy = camera.size.y / 2;
	float camsxzoom = - camsx * ( 1 - 1/camera.zoom );
	float camsyzoom = - camsy * ( 1 - 1/camera.zoom );
	//cout << "camsx : " << camsx << " camsxzoom : " << camsxzoom << " addition : " << camsx + camsxzoom << endl;

	if ( (position.x - 1) * TILE_SIZE <= (camera.position.x + camsx + camsxzoom) && (position.x + size.x + 1) * TILE_SIZE >= (camera.position.x - camsx - camsxzoom) && (position.y - 1) * TILE_SIZE <= (camera.position.y + camsy + camsyzoom) && (position.y + size.y + 1) * TILE_SIZE >= (camera.position.y - camsy - camsyzoom))
	{
		return true;
	}
	return false;
}

Map_Renderer::Map_Renderer(Map* map, Camera* camera)
{
	this->camera = camera;

	for(int i = 0; i < map->tilemaps.size(); i++)
	{
		tilemap_renderers.push_back(Tilemap_Renderer(map, i));
	}

	this->position = {map->x, map->y};
	this->size = {map->width, map->height};
}


void Map_Renderer::Draw(Tileset* tileset)
{
	for (int i = 0; i < tilemap_renderers.size(); i++)
	{
		tilemap_renderers[i].Draw(tileset, *camera);
	}
}

coord_2d Map_Renderer::get_position()
{
	return position;
}

coord_2d Map_Renderer::get_size()
{
	return size;
}

void Camera::Get_View_Size(SDL_Window* window)
{
	SDL_GetWindowSize(window, &size.x, &size.y);
}

Sine_Wave_Generator::Sine_Wave_Generator(int amplitude, int frequency, int phase, int size)
{
	this->amplitude = amplitude;
	this->frequency = frequency;
	this->phase = phase;
	// init wave array
	for (int i = 0; i < size; i++)
	{
		wave.push_back(0);
	}
}

vector<float> Sine_Wave_Generator::get_value(int x)
{
	for (int i = 0; i < wave.size(); i++)
	{
		wave[i] = sin(M_PI * x * frequency + phase) * amplitude;
	}
	return wave;
}

Weather::Weather(Camera* cam, int weather_index)
{
    this->camera = cam;
    this->weather_index = weather_index;
	camx = cam->size.x;
	camy = cam->size.y;
	posx = cam->position.x;
	posy = cam->position.y;
    switch (weather_index)
    {
    case WEATHER_SNOW:
        this->texture = Load_Texture(snow0_ressource, camera->renderer);
		
		for (int i = 0; i < 2000; i++)
        {
            Particle particle;
			
			// apear randomly on the screen 
			particle.posx = rand() % camera->size.x*2 + camera->position.x - camera->size.x / 2;
			particle.posy = rand() % camera->size.y*2 + camera->position.y - camera->size.y / 2;
            particles.push_back(particle);
        }

        break;
    default:
        break;
    }
}

void Weather::Draw()
{
    for (int i = 0; i < particles.size(); i++)
    {
        particles[i].rect.x = (particles[i].posx - camera->position.x) * camera->zoom + camera->size.x / 2;
		particles[i].rect.y = (particles[i].posy - camera->position.y) * camera->zoom + camera->size.y / 2;
        particles[i].rect.w = 8 * camera->zoom;
        particles[i].rect.h = 8 * camera->zoom;
        SDL_RenderCopy(camera->renderer, texture, NULL, &particles[i].rect);
    }
}

void Weather::Update()
{
    switch (weather_index)
    {
    case WEATHER_SNOW:
		// check if camera size has changed
		if (camx != camera->size.x || camy != camera->size.y)
		{
			camx = camera->size.x;
			camy = camera->size.y;
			for (int i = 0; i < particles.size(); i++)
			{
				particles[i].posx = rand() % camera->size.x*2 + camera->position.x - camera->size.x / 2;
				particles[i].posy = rand() % camera->size.y*2 + camera->position.y - camera->size.y / 2;
			}
		}
        for (int i = 0; i < particles.size(); i++)
        {
            particles[i].posx += rand() % 2 - 2;
            particles[i].posy +=  rand() % 2 ;
			if (particles[i].posy > camera->position.y + camera->size.y)
			{
				// apear randomly on the top of the screen
				particles[i].posy = rand() % (camera->size.y/2) - camera->size.y + camera->position.y;
			}
			else if (particles[i].posy < camera->position.y - camera->size.y)
			{
				particles[i].posy = rand() % (camera->size.y/2) + camera->size.y + camera->position.y;
			}
			if (particles[i].posx > camera->position.x + camera->size.x )
			{
				// apear randomly on the left side of the screen ( not visible part )
				particles[i].posx =  rand() % (camera->size.x/2) - camera->size.x + camera->position.x;
			}
			else if (particles[i].posx < camera->position.x - camera->size.x)
			{
				// apear randomly on the right side of the screen
				particles[i].posx =  rand() % (camera->size.x/2) + camera->size.x + camera->position.x;
			}
			
        }
        break;
    default:
        break;
    }
}
