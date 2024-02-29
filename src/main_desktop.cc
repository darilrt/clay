#define CLAY_PLATFORM_DESKTOP
#include "clay.hpp"

int main(int argc, char **argv)
{
    clay::Window *window = clay::Window::create(CLAY_WINDOW_HEIGHT, CLAY_WINDOW_WIDTH, "Clay", nullptr);

    start(window);

    while (window->is_open())
    {
        window->update();
        loop(window);
    }

    return 0;
}