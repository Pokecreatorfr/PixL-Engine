#pragma once
#include <cmath>
#include <Camera.hpp>
#include <const/Config.hpp>



int adjustedSize(int originalSize, float zoom);
int calculateExtraPixels(int originalSize, float zoom);

bool check_visibility(coord_2d position, coord_2d size, Camera* cam);
