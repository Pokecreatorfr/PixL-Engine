#include <Graphics.hpp>


SDL_Texture *Load_Texture(image_ressource image_ressource, SDL_Renderer *renderer)
{
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

	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	if (!texture)
	{
		std::cout << "Failed to create texture\n";
		return nullptr;
	}

	SDL_FreeSurface(surface);

	return texture;
}
