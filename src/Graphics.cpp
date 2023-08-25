#include <Graphics.hpp>

Tileset* Load_Tileset(image_ressource image_ressource, SDL_Renderer* renderer , int tile_width, int tile_height)
{
	Tileset* tileset = new Tileset;
	
	// image ressource is an extern const char image file 
	SDL_RWops* rw = SDL_RWFromConstMem(image_ressource.image_ressource, *image_ressource.image_size);
	if (!rw)
	{
		std::cout << "Failed to create rwops\n";
		return nullptr;
	}

	SDL_Surface* surface = IMG_Load_RW(rw, 1);
	if (!surface)
	{
		std::cout << "Failed to load surface\n";
		return nullptr;
	}


	tileset->tile_width = tile_width;
	tileset->tile_height = tile_height;

	if (surface->w % tile_width != 0 || surface->h % tile_height != 0)
	{
		std::cout << "Tileset width or height is not a multiple of tile width or height\n";
		return nullptr;
	}

	// number of tiles in tileset
	tileset->tileset_width = surface->w / tile_width;
	tileset->tileset_height = surface->h / tile_height;

	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	if (!texture)
	{
		std::cout << "Failed to create texture\n";
		return nullptr;
	}
	tileset->texture = texture;

	for (int i = 0; i < tileset->tileset_height; i++)
	{
		for (int j = 0; j < tileset->tileset_width; j++)
		{
			SDL_Rect rect;
			rect.x = j * tile_height;
			rect.y = i * tile_width;
			rect.w = tile_width;
			rect.h = tile_height;
			tileset->tiles.push_back(rect);
		}
	}


	SDL_FreeSurface(surface);

	return tileset;
}