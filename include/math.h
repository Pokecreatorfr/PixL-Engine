#pragma once
#include <cmath>



int adjustedSize(int originalSize, float zoom);
int calculateExtraPixels(int originalSize, float zoom);

bool check_visibility(int camx, int camy, int camw, int camh, int rectx, int recty, int rectw, int recth);
