#include <Graphics.hpp>


SDL_Texture *Load_Texture(image_ressource image_ressource, SDL_Renderer *renderer)
{
	SDL_RWops* rw = SDL_RWFromConstMem(image_ressource.image_ressource, *image_ressource.image_size);
	if (!rw)
	{
		std::cout << "Failed to create rwops\n";
		return nullptr;
	}

	SDL_Texture* texture = IMG_LoadTexture_RW(renderer, rw, 1);
	if (!texture)
	{
		std::cout << "Failed to create texture\n";
		return nullptr;
	}

	return texture;
}
