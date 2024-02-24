#pragma once
#include <vector>
#include <GlobalsStructs.hpp>

#define HEIGHT 1
#define WIDTH 0

struct int_to_adjust
{
    int* coord;
    int type;
    float size;
    int uid; // unique id set by the renderer
};

class CoordCalculator
{
    public:
        CoordCalculator(camera* cam);
        ~CoordCalculator();
        int add_coord_to_adjust(int* coord, int type, float size);
        void remove_coord_to_adjust(int uid);
        void adjust_coords();
    private:
        std::vector<int_to_adjust*> int_to_adjusts;
        camera* Camera;
};