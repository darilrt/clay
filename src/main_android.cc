#define CLAY_PLATFORM_ANDROID
#include "clay.hpp"

#include <jni.h>

#include <game-activity/GameActivity.cpp>
#include <game-text-input/gametextinput.cpp>
#include <game-activity/native_app_glue/android_native_app_glue.c>

extern "C"
{
    void cmd(struct android_app *app, int32_t cmd)
    {
        switch (cmd)
        {
        case APP_CMD_INIT_WINDOW:
        {
            app->userData = clay::Window::create(-1, -1, "Clay", app);
            start(app->userData);
            break;
        }
        case APP_CMD_TERM_WINDOW:
        {
            break;
        }
        case APP_CMD_DESTROY:
        {
            clay::Window *window = (clay::Window *)app->userData;
            delete window;
            break;
        }
        }
    }

    void android_main(struct android_app *app)
    {
        app->onAppCmd = cmd;

        android_app_set_motion_event_filter(
            app,
            [](const GameActivityMotionEvent *motionEvent)
            {
                auto sourceClass = motionEvent->source & AINPUT_SOURCE_CLASS_MASK;
                return (sourceClass == AINPUT_SOURCE_CLASS_POINTER ||
                        sourceClass == AINPUT_SOURCE_CLASS_JOYSTICK);
            });

        int events;
        android_poll_source *pSource;
        do
        {
            if (app->userData != nullptr)
            {
                clay::Window *window = (clay::Window *)app->userData;

                if (window->is_open())
                {
                    window->update();
                    loop(window);
                }
            }
            else
            {
                while (ALooper_pollAll(0, NULL, &events, (void **)&pSource) >= 0)
                {
                    if (pSource != NULL)
                    {
                        pSource->process(app, pSource);
                    }
                }
            }
        } while (app->destroyRequested == 0);
    }
}
