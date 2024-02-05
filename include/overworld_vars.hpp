#pragma once

struct coord_2d
{
    int x;
    int y;
};

struct Camera
{
    struct coord_2d size;
    struct coord_2d position;
    float zoom;
};

struct time_struct
{
    int hours;
    int minutes;
    int seconds;
};

struct Tile
{
    Tile(coord_2d position, coord_2d size, int tile_index) : position(position), size(size), tile_index(tile_index) {}
    coord_2d position;
    coord_2d size;
    int tile_index;
};


struct overworld_vars
{
    int animations_ticks;
    time_struct time;
    Camera camera;
};

