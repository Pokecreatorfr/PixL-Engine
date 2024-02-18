#pragma once
#include <Graphics.hpp>

struct font{
    const int size;
    const image_ressource* font_image;
};

struct font_ressource{
    int uid;
    vector<const font*> fonts;
};