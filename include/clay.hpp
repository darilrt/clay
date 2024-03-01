//
// Description: A simple cross-platform game development library.
// Website:
//
// License: MIT
//
// This file is part of clay.
//
// Clay is free software: you can redistribute it and/or modify
// it under the terms of the MIT License.
//
// Clay is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// MIT License for more details.
//
// You should have received a copy of the MIT License along with Clay.
// If not, see <https://opensource.org/licenses/MIT>.
//
// clay.hpp is a header-only library that provides a simple cross-platform game development library.
// It is designed to be easy to use and easy to understand, while providing a simple and efficient API.
// It is written in C++ and is designed to be used with C++.
//
// The library is designed to be used with the following platforms:
// - Android
// - Desktop
//
// The library is designed to be used with the following graphics APIs:
// - OpenGL ES 3.0
// - OpenGL 3.3
// - WebGL 2.0 (TODO)
//
// Usage:
// - Include the clay.hpp header file in your project.
// - Use the clay::Window class to create a window.
// - Use the clay::Window::update method to update the window.
// - Use the clay::Window::is_open method to check if the window is open.
// - Use the clay::Window::~Window destructor to destroy the window.
// - Use the CLAY_RUN macro to run your app.
//
// Example:
//     #define CLAY_PLATFORM_DESKTOP // Define the platform as desktop or android (CLAY_PLATFORM_ANDROID)
//     #include <clay.hpp>
//
//     class App
//     {
//     public:
//         bool is_running = true;
//         clay::Window *window;
//
//         App(void *app)
//         {
//             window = clay::Window::create(640, 480, "Clay", app);
//         }
//
//         void update()
//         {
//             window->update();
//             is_running = window->is_open();
//         }
//     };
//
//     CLAY_RUN(App);
//

#pragma once

#include <cstdint>
#include <string>

typedef void *id;

namespace debug
{
    void log(const char *message);

    void log(const std::string &message);
} // namespace debug

namespace clay
{
    class Window
    {
    public:
        int32_t width;
        int32_t height;
        id window;
        id context;
        id surface;
        id app;
        bool is_quit = true;
        id event;

        static Window *create(int width = -1, int height = -1, const char *title = "", void *app_ = nullptr); // Create a window with the given width, height, and title (desktop) or app (android)

        void update(); // Update the window

        inline bool is_open() { return !is_quit; } // Check if the window is open

        ~Window(); // Destructor
    };
} // namespace clay

#ifdef CLAY_PLATFORM_ANDROID
#include <game-activity/native_app_glue/android_native_app_glue.h>

#define CLAY_RUN(APP)                                                                                                   \
    extern "C"                                                                                                          \
    {                                                                                                                   \
        void _handle_cmd(struct android_app *app, int32_t cmd)                                                          \
        {                                                                                                               \
            switch (cmd)                                                                                                \
            {                                                                                                           \
            case APP_CMD_INIT_WINDOW:                                                                                   \
            {                                                                                                           \
                app->userData = new APP(app);                                                                           \
                break;                                                                                                  \
            }                                                                                                           \
            case APP_CMD_TERM_WINDOW:                                                                                   \
            {                                                                                                           \
                break;                                                                                                  \
            }                                                                                                           \
            case APP_CMD_DESTROY:                                                                                       \
            {                                                                                                           \
                delete (APP *)app->userData;                                                                            \
                break;                                                                                                  \
            }                                                                                                           \
            }                                                                                                           \
        }                                                                                                               \
        void android_main(struct android_app *app)                                                                      \
        {                                                                                                               \
            app->onAppCmd = _handle_cmd;                                                                                \
            android_app_set_motion_event_filter(                                                                        \
                app,                                                                                                    \
                [](const GameActivityMotionEvent *motionEvent)                                                          \
                {                                                                                                       \
                    auto sourceClass = motionEvent->source & AINPUT_SOURCE_CLASS_MASK;                                  \
                    return (sourceClass == AINPUT_SOURCE_CLASS_POINTER || sourceClass == AINPUT_SOURCE_CLASS_JOYSTICK); \
                });                                                                                                     \
            int events;                                                                                                 \
            android_poll_source *pSource;                                                                               \
            do                                                                                                          \
            {                                                                                                           \
                while (ALooper_pollAll(0, nullptr, &events, (void **)&pSource) >= 0)                                    \
                {                                                                                                       \
                    if (pSource != nullptr)                                                                             \
                    {                                                                                                   \
                        pSource->process(app, pSource);                                                                 \
                    }                                                                                                   \
                }                                                                                                       \
                if (app->userData != nullptr)                                                                           \
                {                                                                                                       \
                    ((APP *)(app->userData))->update();                                                                 \
                }                                                                                                       \
            } while (app->destroyRequested == 0);                                                                       \
        }                                                                                                               \
    }

#else

namespace clay
{
    template <typename T>
    void _run()
    {
        T *app = new T(nullptr);
        while (app->is_running)
        {
            app->update();
        }
        delete app;
    }
}

#define CLAY_RUN(app)      \
    int main()             \
    {                      \
        clay::_run<app>(); \
    }

#endif

#ifdef CLAY_IMPLEMENTATION
#ifdef CLAY_PLATFORM_ANDROID
#include <android/log.h>
#include <game-activity/native_app_glue/android_native_app_glue.h>
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <memory>
#include <vector>

#include <game-activity/native_app_glue/android_native_app_glue.c>
#include <game-activity/GameActivity.cpp>
#include <game-text-input/gametextinput.cpp>

namespace debug
{
    void log(const char *message)
    {
        __android_log_print(ANDROID_LOG_INFO, "Clay", "%s", message);
    }

    void log(const std::string &message)
    {
        __android_log_print(ANDROID_LOG_INFO, "Clay", "%s", message.c_str());
    }
}

namespace clay
{
    Window *Window::create(int width, int height, const char *title, void *_app_)
    {
        struct android_app *app_ = (struct android_app *)_app_;

        debug::log("Creating window");

        constexpr EGLint attribs[] = {
            EGL_RENDERABLE_TYPE,
            EGL_OPENGL_ES3_BIT,
            EGL_SURFACE_TYPE,
            EGL_WINDOW_BIT,
            EGL_BLUE_SIZE,
            8,
            EGL_GREEN_SIZE,
            8,
            EGL_RED_SIZE,
            8,
            EGL_DEPTH_SIZE,
            24,
            EGL_NONE,
        };

        // The default display is probably what you want on Android
        auto display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        eglInitialize(display, nullptr, nullptr);

        // figure out how many configs there are
        EGLint numConfigs;
        eglChooseConfig(display, attribs, nullptr, 0, &numConfigs);

        // get the list of configurations
        std::unique_ptr<EGLConfig[]> supportedConfigs(new EGLConfig[numConfigs]);
        eglChooseConfig(display, attribs, supportedConfigs.get(), numConfigs, &numConfigs);

        // Find a config we like.
        // Could likely just grab the first if we don't care about anything else in the config.
        // Otherwise hook in your own heuristic
        auto config = *std::find_if(
            supportedConfigs.get(),
            supportedConfigs.get() + numConfigs,
            [&display](const EGLConfig &config)
            {
                EGLint red, green, blue, depth;
                if (eglGetConfigAttrib(display, config, EGL_RED_SIZE, &red) && eglGetConfigAttrib(display, config, EGL_GREEN_SIZE, &green) && eglGetConfigAttrib(display, config, EGL_BLUE_SIZE, &blue) && eglGetConfigAttrib(display, config, EGL_DEPTH_SIZE, &depth))
                {
                    return red == 8 && green == 8 && blue == 8 && depth == 24;
                }
                return false;
            });

        // create the proper window surface
        EGLint format;
        eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);
        EGLSurface surface = eglCreateWindowSurface(display, config, app_->window, nullptr);

        // Create a GLES 3 context
        EGLint contextAttribs[] = {EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE};
        EGLContext context = eglCreateContext(display, config, nullptr, contextAttribs);

        // get some window metrics
        auto madeCurrent = eglMakeCurrent(display, surface, surface, context);

        if (madeCurrent == EGL_FALSE)
        {
            debug::log("Could not make the context current");
        }
        else
        {
            debug::log("Context made current");
        }

        debug::log((const char *)glGetString(GL_VENDOR));
        debug::log((const char *)glGetString(GL_RENDERER));
        debug::log((const char *)glGetString(GL_VERSION));
        debug::log((const char *)glGetString(GL_EXTENSIONS));

        return new Window{
            -1,
            -1,
            display,
            context,
            surface,
            _app_,
            false,
        };
    }

    void Window::update()
    {
        eglSwapBuffers((EGLDisplay)window, (EGLSurface)surface);

        EGLint _width;
        eglQuerySurface(window, surface, EGL_WIDTH, &_width);

        EGLint _height;
        eglQuerySurface(window, surface, EGL_HEIGHT, &_height);

        if (width != _width || height != _height)
        {
            width = _width;
            height = _height;
            glViewport(0, 0, width, height);
        }

        int32_t events;
        struct android_poll_source *source;

        is_quit = ((android_app *)app)->destroyRequested;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    Window::~Window()
    {
        eglMakeCurrent((EGLDisplay)window, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        eglDestroySurface((EGLDisplay)window, (EGLSurface)surface);
        eglDestroyContext((EGLDisplay)window, (EGLContext)context);
        eglTerminate((EGLDisplay)window);
    }
} // namespace clay

#else // CLAY_PLATFORM_DESKTOP
#include <glad/glad.h>
#include <GL/gl.h>
#include <SDL2/SDL.h>
#include <iostream>

namespace debug
{
    void log(const char *message)
    {
        std::cout << message << std::endl;
    }

    void log(const std::string &message)
    {
        std::cout << message << std::endl;
    }
}

namespace clay
{
    Window *Window::create(int width, int height, const char *title, void *app_)
    {
        debug::log("Creating window");

        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0)
        {
            debug::log("SDL could not initialize! SDL_Error: " + std::string(SDL_GetError()));
        }
        else
        {
            debug::log("SDL initialized");
        }

        SDL_Window *window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_OPENGL);

        if (window == NULL)
        {
            debug::log("Window could not be created! SDL_Error: " + std::string(SDL_GetError()));
        }
        else
        {
            debug::log("Window created");
        }

        SDL_GLContext context = SDL_GL_CreateContext(window);

        if (context == NULL)
        {
            debug::log("OpenGL context could not be created! SDL_Error: " + std::string(SDL_GetError()));
        }
        else
        {
            debug::log("OpenGL context created");
        }

        if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
        {
            debug::log("Failed to initialize GLAD");
        }
        else
        {
            debug::log("GLAD initialized");
        }

        glClearColor(0.03f, 0.03f, 0.04f, 1.0f);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glClear(GL_COLOR_BUFFER_BIT);

        SDL_GL_SwapWindow(window);

        return new Window{
            width,
            height,
            window,
            context,
            nullptr,
            nullptr,
            false,
            new SDL_Event(),
        };
    }

    void Window::update()
    {
        SDL_GL_SwapWindow((SDL_Window *)window);

        int32_t _width;
        int32_t _height;
        SDL_GetWindowSize((SDL_Window *)window, &_width, &_height);

        if (width != _width || height != _height)
        {
            width = _width;
            height = _height;
            glViewport(0, 0, width, height);
        }

        while (SDL_PollEvent((SDL_Event *)event))
        {
            if (((SDL_Event *)event)->type == SDL_QUIT)
            {
                is_quit = true;
            }
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    Window::~Window()
    {
        SDL_GL_DeleteContext((SDL_GLContext)context);
        SDL_DestroyWindow((SDL_Window *)window);
        SDL_Quit();
    }
}

#endif
#endif