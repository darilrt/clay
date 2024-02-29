#ifdef __ANDROID__
#define CLAY_PLATFORM_ANDROID
#else
#define CLAY_PLATFORM_DESKTOP
#endif

#define CLAY_IMPLEMENTATION
#include "clay.hpp"

#define CLAY_MATH_IMPLEMENTATION
#include "clay_math.hpp"