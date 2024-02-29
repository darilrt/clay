#pragma once

#include <cstdint>
#include <string>

typedef uint32_t glid;
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

namespace gfx
{
    void clear_color(float r, float g, float b, float a);                                                          // Set the clear color
    void clear();                                                                                                  // Clear the screen or the framebuffer
    void draw(size_t vertex_count, size_t instance_count = 1, size_t first_vertex = 0, size_t first_instance = 0); // Draw the vertices
    void viewport(float x, float y, float width, float height);
    void enable_depth_test(bool enable);
    void enable_backface_culling(bool enable);
    void enable_blending(bool enable);
    void unbind_framebuffer();

    enum class ShaderType : uint32_t
    {
        Vertex = 0x8B31,
        Fragment = 0x8B30,
        Geometry = 0x8DD9,
        TessControl = 0x8E88,
        TessEvaluation = 0x8E87,
        Compute = 0x91B9
    };

    class ShaderModule // Shader module
    {
    public:
        glid id = 0; // Shader module id

        ShaderModule(ShaderType type);

        void set_source(const char *source); // Set the source of the shader module

        void compile();

        ~ShaderModule(); // Destructor
    };

    class Attribute
    {
    public:
        glid id = 0;

        void set_pointer(void *data, size_t size, size_t stride);

        void enable();

        void disable();
    };

    class Uniform
    {
    public:
        glid id = 0;

        void set_float(float value);

        void set_int(int value);

        void set_vec2(float x, float y);

        void set_vec3(float x, float y, float z);

        void set_vec4(float x, float y, float z, float w);

        void set_mat2(float *value);

        void set_mat3(float *value);

        void set_mat4(float *value);
    };

    class Pipeline // Pipeline
    {
    public:
        glid id = 0; // Pipeline id

        Pipeline(); // Constructor

        void attach_shader(const ShaderModule &shader); // Attach a shader module to the pipeline

        glid get_uniform_location(const char *name); // Get the location of a uniform

        Attribute get_attribute(const char *name); // Get the location of an attribute

        Uniform get_uniform(const char *name); // Get the location of a uniform

        void link(); // Link the pipeline

        void use(); // Use the pipeline

        ~Pipeline(); // Destructor
    };

    enum class BufferType : uint32_t
    {
        Array = 0x8892,
        ElementArray = 0x8893,
        Uniform = 0x8A11,
        ShaderStorage = 0x90D2
    };

    enum class BufferUsage : uint32_t
    {
        Static = 0x88E4,
        Dynamic = 0x88E8,
        Stream = 0x88E0
    };

    class Buffer
    {
    public:
        glid id = 0;
        BufferType type;
        void *data;

        Buffer(BufferType type);
        ~Buffer();

        void bind();
        void unbind();

        void set_data(const void *data, size_t size, BufferUsage usage);
        void set_sub_data(const void *data, size_t size, size_t offset);

        void map();
        void unmap();
    };

    class VertexArray
    {
    public:
        glid id = 0;

        VertexArray();
        ~VertexArray();

        void bind();
        void unbind();

        void set_attribute(Buffer &buffer, size_t index, size_t size, size_t stride, size_t offset);
        void set_index_buffer(Buffer &buffer);
    };
}

#ifdef CLAY_IMPLEMENTATION

#ifdef CLAY_PLATFORM_ANDROID
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#else
#include <glad/glad.h>
#include <GL/gl.h>
#endif // CLAY_PLATFORM_ANDROID

void CheckOpenGLError(const char *stmt, const char *fname, int line);

#ifdef _DEBUG
#define GL_CALL(stmt)                                \
    do                                               \
    {                                                \
        stmt;                                        \
        CheckOpenGLError(#stmt, __FILE__, __LINE__); \
    } while (0)
#else
#define GL_CALL(stmt) stmt
#endif

namespace gfx
{
    void clear_color(float r, float g, float b, float a)
    {
        GL_CALL(glClearColor(r, g, b, a));
    }

    void clear()
    {
        GL_CALL(glClear(GL_COLOR_BUFFER_BIT));
    }

    void draw(size_t vertex_count, size_t instance_count, size_t first_vertex, size_t first_instance)
    {
        if (instance_count <= 1)
        {
            GL_CALL(glDrawArrays(GL_TRIANGLES, first_vertex, vertex_count));
        }
        else
        {
            GL_CALL(glDrawArraysInstanced(GL_TRIANGLES, first_vertex, vertex_count, instance_count));
        }
    }

    void viewport(float x, float y, float width, float height)
    {
        glViewport(x, y, width, height);
    }

    void enable_depth_test(bool enable)
    {
        if (enable)
        {
            glEnable(GL_DEPTH_TEST);
        }
        else
        {
            glDisable(GL_DEPTH_TEST);
        }
    }

    void enable_backface_culling(bool enable)
    {
        if (enable)
        {
            glEnable(GL_CULL_FACE);
        }
        else
        {
            glDisable(GL_CULL_FACE);
        }
    }

    void enable_blending(bool enable)
    {
        if (enable)
        {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
        else
        {
            glDisable(GL_BLEND);
        }
    }

    void unbind_framebuffer()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    ShaderModule::ShaderModule(ShaderType type)
    {
        id = glCreateShader((GLenum)type);
    }

    void ShaderModule::set_source(const char *source)
    {
        GL_CALL(glShaderSource(id, 1, &source, NULL));
    }

    void ShaderModule::compile()
    {
        GL_CALL(glCompileShader(id));

        int success;
        char infoLog[512];

        GL_CALL(glGetShaderiv(id, GL_COMPILE_STATUS, &success));

        if (!success)
        {
            GL_CALL(glGetShaderInfoLog(id, 512, NULL, infoLog));
            debug::log("Shader compilation failed: " + std::string(infoLog));
        }
    }

    ShaderModule::~ShaderModule()
    {
        GL_CALL(glDeleteShader(id));
    }

    void Attribute::set_pointer(void *data, size_t size, size_t stride)
    {
        GL_CALL(glVertexAttribPointer(id, size, GL_FLOAT, GL_FALSE, stride, data));
    }

    void Attribute::enable()
    {
        GL_CALL(glEnableVertexAttribArray(id));
    }

    void Attribute::disable()
    {
        GL_CALL(glDisableVertexAttribArray(id));
    }

    void Uniform::set_float(float value)
    {
        GL_CALL(glUniform1f(id, value));
    }

    void Uniform::set_int(int value)
    {
        GL_CALL(glUniform1i(id, value));
    }

    void Uniform::set_vec2(float x, float y)
    {
        GL_CALL(glUniform2f(id, x, y));
    }

    void Uniform::set_vec3(float x, float y, float z)
    {
        GL_CALL(glUniform3f(id, x, y, z));
    }

    void Uniform::set_vec4(float x, float y, float z, float w)
    {
        GL_CALL(glUniform4f(id, x, y, z, w));
    }

    void Uniform::set_mat2(float *value)
    {
        GL_CALL(glUniformMatrix2fv(id, 1, GL_FALSE, value));
    }

    void Uniform::set_mat3(float *value)
    {
        GL_CALL(glUniformMatrix3fv(id, 1, GL_FALSE, value));
    }

    void Uniform::set_mat4(float *value)
    {
        GL_CALL(glUniformMatrix4fv(id, 1, GL_FALSE, value));
    }

    Pipeline::Pipeline()
    {
        id = glCreateProgram();
    }

    void Pipeline::attach_shader(const ShaderModule &shader)
    {
        GL_CALL(glAttachShader(id, shader.id));
    }

    glid Pipeline::get_uniform_location(const char *name)
    {
        return glGetUniformLocation(id, name);
    }

    Attribute Pipeline::get_attribute(const char *name)
    {
        Attribute attribute;
        attribute.id = glGetAttribLocation(id, name);
        return attribute;
    }

    Uniform Pipeline::get_uniform(const char *name)
    {
        Uniform uniform;
        uniform.id = glGetUniformLocation(id, name);
        return uniform;
    }

    void Pipeline::link()
    {
        GL_CALL(glLinkProgram(id));

        int success;
        char infoLog[512];

        GL_CALL(glGetProgramiv(id, GL_LINK_STATUS, &success));

        if (!success)
        {
            GL_CALL(glGetProgramInfoLog(id, 512, NULL, infoLog));
            debug::log("Pipeline linking failed: " + std::string(infoLog));
        }
    }

    void Pipeline::use()
    {
        GL_CALL(glUseProgram(id));
    }

    Pipeline::~Pipeline()
    {
        GL_CALL(glDeleteProgram(id));
    }

    Buffer::Buffer(BufferType type)
    {
        this->type = type;
        GL_CALL(glGenBuffers(1, &id));
    }

    Buffer::~Buffer()
    {
        GL_CALL(glDeleteBuffers(1, &id));
    }

    void Buffer::bind()
    {
        GL_CALL(glBindBuffer((GLenum)type, id));
    }

    void Buffer::unbind()
    {
        GL_CALL(glBindBuffer((GLenum)type, 0));
    }

    void Buffer::set_data(const void *data, size_t size, BufferUsage usage)
    {
        this->bind();
        GL_CALL(glBufferData((GLenum)type, size, data, (GLenum)usage));
        this->unbind();
    }

    void Buffer::set_sub_data(const void *data, size_t size, size_t offset)
    {
        this->bind();
        GL_CALL(glBufferSubData((GLenum)type, offset, size, data));
        this->unbind();
    }

    void Buffer::map()
    {
        bind();
        data = glMapBufferRange((GLenum)type, 0, 0, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
    }

    void Buffer::unmap()
    {
        bind();
        GL_CALL(glUnmapBuffer((GLenum)type));
        data = nullptr;
        unbind();
    }

    VertexArray::VertexArray()
    {
        GL_CALL(glGenVertexArrays(1, &id));
    }

    VertexArray::~VertexArray()
    {
        GL_CALL(glDeleteVertexArrays(1, &id));
    }

    void VertexArray::bind()
    {
        GL_CALL(glBindVertexArray(id));
    }

    void VertexArray::unbind()
    {
        GL_CALL(glBindVertexArray(0));
    }

    void VertexArray::set_attribute(Buffer &buffer, size_t index, size_t size, size_t stride, size_t offset)
    {
        bind();
        buffer.bind();
        GL_CALL(glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, stride, (void *)offset));
        GL_CALL(glEnableVertexAttribArray(index));
        buffer.unbind();
        unbind();
    }

    void VertexArray::set_index_buffer(Buffer &buffer)
    {
        bind();
        buffer.bind();
        unbind();
    }

}

#ifdef CLAY_PLATFORM_ANDROID
#include <android/log.h>
#include <game-activity/native_app_glue/android_native_app_glue.h>
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <memory>
#include <vector>

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

        while (ALooper_pollAll(0, NULL, &events, (void **)&source) >= 0)
        {
            if (source != NULL)
            {
                source->process((android_app *)app, source);
            }
        }

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
}

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