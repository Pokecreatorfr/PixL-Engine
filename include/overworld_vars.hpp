#pragma once

struct coord_2d
{
    int x;
    int y;
};

struct camera
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

struct overworld_vars
{
    int animations_ticks;
    time_struct time;
    camera camera;
};

