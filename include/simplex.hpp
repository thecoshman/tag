#pragma once
#include <cmath>

namespace util{
    float simplex_noise(int octaves, float x, float y, float z);

    float simplex_noise(int octaves, float x, float y);
}