#pragma once

struct camera_struct
{
    int x;
    int y;
    float zoom;
    int width;
    int height;
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
    camera_struct camera;
};
