#pragma once
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <Config.hpp>
#include <Graphics.hpp>
#include <cmath>

using namespace std;


struct Camera
{
	Camera(coord_2d position, SDL_Renderer* renderer, coord_2d size, float zoom) : position(position), renderer(renderer), size(size), zoom(zoom) {}
	void Get_View_Size(SDL_Window* window);
	coord_2d position;
	SDL_Renderer* renderer;
	coord_2d size;
	float zoom = 1.0f;
};

struct Map
{
	int height;
	int width;
	int x;
	int y;
	vector<vector<int>> tilemaps;
	vector<int> collision;
};

struct World
{
	const coord_2d position;
	const coord_2d size;
	vector<Map> maps;
};


class Tilemap_Renderer
{
public:
	Tilemap_Renderer(Map* map ,int layer);
	void Draw(Tileset* tileset,Camera camera);
private:
	vector<Tile> tiles;
};

class Map_Renderer
{
	public:
		Map_Renderer(Map* map,Camera* camera);
		void Draw(Tileset* tileset);
		coord_2d get_position();
		coord_2d get_size();
	private:
		vector<Tilemap_Renderer> tilemap_renderers;
		Camera* camera;
		coord_2d position;
		coord_2d size;
};

class World_Renderer
{
	public:
		World_Renderer(World* world , Camera* cam);
		~World_Renderer();
		void Draw(Tileset* tileset);
		void update();
		void load_tileset(const char* file_path, SDL_Renderer* renderer, int tile_width, int tile_height);
	private:
		bool check_visibility(coord_2d position, coord_2d size, Camera camera);
		World* world;
		Map_Renderer* Map_Renderers[5] = { nullptr, nullptr , nullptr , nullptr };
		Camera* camera;
};

class Sine_Wave_Generator
{
	public:
		Sine_Wave_Generator(int amplitude, int frequency, int phase,int size);
		vector<float> get_value(int x);
	private:
		int amplitude;
		int frequency;
		int phase;
		vector<float> wave;
};
