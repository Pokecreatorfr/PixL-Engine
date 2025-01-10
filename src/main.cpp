#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <vector>

// Structures
struct Tileset
{
	std::string path;
	uint16_t numberTileHeight; // Hauteur d'une tuile en pixels
	uint16_t numberTileWidth;  // Largeur d'une tuile en pixels
};

struct Tile
{
	uint16_t index; // Index de la tuile dans le tileset
	uint8_t alpha;	// Transparence (0 = transparent, 255 = opaque)
#ifdef OPTIMIZE_SIZE
	bool flipX : 1; // Retournement horizontal
	bool flipY : 1; // Retournement vertical
#else
	bool flipX;
	bool flipY;
#endif
};

struct Tilemap
{
	uint16_t width;			 // Largeur de la tilemap en tuiles
	uint16_t height;		 // Hauteur de la tilemap en tuiles
	std::vector<Tile> tiles; // Liste des tuiles (taille = width * height)
};

struct Camera
{
	uint16_t x;		 // Position x de la caméra
	uint16_t y;		 // Position y de la caméra
	uint16_t width;	 // Largeur de la zone visible
	uint16_t height; // Hauteur de la zone visible
	float zoom;		 // Facteur de zoom
};

// Charger un tileset
SDL_Texture *loadTileset(SDL_Renderer *renderer, const std::string &path)
{
	SDL_Surface *surface = IMG_Load(path.c_str());
	if (!surface)
	{
		SDL_Log("Erreur : Impossible de charger le fichier %s, %s", path.c_str(), IMG_GetError());
		return nullptr;
	}
	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);
	return texture;
}

// Fonction pour dessiner une Tilemap
void drawTilemap(SDL_Renderer *renderer, SDL_Texture *tilesetTexture,
				 const Tileset &tileset, const Tilemap &tilemap, const Camera &camera)
{
	// Dimensions du tileset
	int tilesetWidth, tilesetHeight;
	SDL_QueryTexture(tilesetTexture, nullptr, nullptr, &tilesetWidth, &tilesetHeight);

	// Taille d'une tuile normalisée
	float tileWidthNorm = static_cast<float>(tileset.numberTileWidth) / tilesetWidth;
	float tileHeightNorm = static_cast<float>(tileset.numberTileHeight) / tilesetHeight;

	// Nombre de colonnes dans le tileset
	int tilesetCols = tilesetWidth / tileset.numberTileWidth;

	// Parcourir la caméra (les tuiles visibles)
	for (int y = 0; y < camera.height / tileset.numberTileHeight; ++y)
	{
		for (int x = 0; x < camera.width / tileset.numberTileWidth; ++x)
		{
			// Calcul des indices de la tuile dans la tilemap
			int mapX = camera.x / tileset.numberTileWidth + x;
			int mapY = camera.y / tileset.numberTileHeight + y;

			if (mapX >= tilemap.width || mapY >= tilemap.height)
				continue; // Hors de la tilemap

			int tileIndex = mapY * tilemap.width + mapX;
			const Tile &tile = tilemap.tiles[tileIndex];

			// Calcul des coordonnées de la source normalisées
			float srcXNorm = (tile.index % tilesetCols) * tileWidthNorm;
			float srcYNorm = (tile.index / tilesetCols) * tileHeightNorm;

			SDL_Rect srcRect = {
				static_cast<int>(srcXNorm * tilesetWidth),
				static_cast<int>(srcYNorm * tilesetHeight),
				static_cast<int>(tileWidthNorm * tilesetWidth),
				static_cast<int>(tileHeightNorm * tilesetHeight)};

			// Calcul de la destination sur l'écran
			int destX = x * tileset.numberTileWidth - (camera.x % tileset.numberTileWidth);
			int destY = y * tileset.numberTileHeight - (camera.y % tileset.numberTileHeight);
			SDL_Rect destRect = {destX, destY, static_cast<int>(tileset.numberTileWidth * camera.zoom), static_cast<int>(tileset.numberTileHeight * camera.zoom)};

			// Options de rendu (flips, alpha)
			SDL_RendererFlip flip = SDL_FLIP_NONE;
			if (tile.flipX)
				flip = (SDL_RendererFlip)(flip | SDL_FLIP_HORIZONTAL);
			if (tile.flipY)
				flip = (SDL_RendererFlip)(flip | SDL_FLIP_VERTICAL);

			// Appliquer l'alpha
			SDL_SetTextureAlphaMod(tilesetTexture, tile.alpha);

			// Rendu de la tuile
			SDL_RenderCopyEx(renderer, tilesetTexture, &srcRect, &destRect, 0.0, nullptr, flip);
		}
	}
}

int main(int argc, char *argv[])
{
	// Initialisation SDL
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		SDL_Log("Erreur SDL_Init : %s", SDL_GetError());
		return -1;
	}
	if (IMG_Init(IMG_INIT_PNG) == 0)
	{
		SDL_Log("Erreur IMG_Init : %s", IMG_GetError());
		return -1;
	}

	SDL_Window *window = SDL_CreateWindow("Tilemap Viewer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
	if (!window)
	{
		SDL_Log("Erreur SDL_CreateWindow : %s", SDL_GetError());
		return -1;
	}

	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!renderer)
	{
		SDL_Log("Erreur SDL_CreateRenderer : %s", SDL_GetError());
		return -1;
	}

	// Charger un tileset
	Tileset tileset = {"data/Tilesets/main_tileset.png", 32, 32};
	SDL_Texture *tilesetTexture = loadTileset(renderer, tileset.path);
	if (!tilesetTexture)
	{
		return -1;
	}

	// Créer une Tilemap
	Tilemap tilemap = {32, 32, {}};
	for (int i = 0; i < tileset.numberTileHeight * tileset.numberTileWidth; ++i)
	{
		Tile tile = {i, 255, false, false};
		tilemap.tiles.push_back(tile);
	}

	// Configurer une caméra
	Camera camera = {10, 10, 800, 600, 1.0f};

	// Boucle principale
	bool running = true;
	SDL_Event event;
	while (running)
	{
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				running = false;
			}
		}

		// Effacer l'écran
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		// Dessiner la Tilemap
		drawTilemap(renderer, tilesetTexture, tileset, tilemap, camera);

		// Présenter le rendu
		SDL_RenderPresent(renderer);
	}

	// Nettoyage
	SDL_DestroyTexture(tilesetTexture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	IMG_Quit();
	SDL_Quit();

	return 0;
}
