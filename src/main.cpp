#ifdef __ANDROID__
#define CLAY_PLATFORM_ANDROID
#include <game-activity/native_app_glue/android_native_app_glue.h>
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <sensors.h>
#else
#define CLAY_PLATFORM_DESKTOP
#include <glad/glad.h>
#include <GL/gl.h>
#endif

#include "clay_math.hpp"
#include "clay_gfx.hpp"
#include "clay.hpp"

#include "mesh.hpp"

#include <vector>
#include <memory>

#define CLAY_WINDOW_WIDTH 800
#define CLAY_WINDOW_HEIGHT 600

class App
{
public:
    bool is_running = true;
    clay::Window *window;
    std::unique_ptr<gfx::Pipeline> pipeline;
    gfx::Uniform uniform_proj;
    Mesh *mesh;

    App(void *app)
    {
        window = clay::Window::create(CLAY_WINDOW_WIDTH, CLAY_WINDOW_HEIGHT, "Clay", app);

        gfx::ShaderModule vertex_shader(gfx::ShaderType::Vertex);
        vertex_shader.set_source(R"(#version 300 es
            layout(location=0) in vec3 position;
            layout(location=1) in vec3 uv;

            uniform mat4 proj;

            void main() {
                gl_Position = proj * vec4(position, 1.0);
            }
        )");
        vertex_shader.compile();

        gfx::ShaderModule fragment_shader(gfx::ShaderType::Fragment);
        fragment_shader.set_source(R"(#version 300 es
            precision mediump float;

            out vec4 out_color;

            void main() {
                out_color = vec4(1.0, 1.0, 1.0, 1.0);
            }
        )");
        fragment_shader.compile();

        pipeline = std::make_unique<gfx::Pipeline>();
        pipeline->attach_shader(vertex_shader);
        pipeline->attach_shader(fragment_shader);
        pipeline->link();

        uniform_proj = pipeline->get_uniform("proj");

        mesh = new Mesh({-0.5, -0.5, 0.0,
                         0.0, 0.5, 0.0,
                         0.5, -0.5, 0.0},
                        {0.0, 0.0,
                         0.5, 1.0,
                         1.0, 0.0});
    }

    void update()
    {
        float aspect = (float)window->height / (float)window->width;
        Mat4 proj = Mat4::ortho(1.0, -1.0, -aspect, aspect, 1.0f, -1.0f);

        pipeline->use();
        uniform_proj.set_mat4(&proj.data[0]);

        mesh->draw();

        window->update();
        is_running = window->is_open();
    }
};

CLAY_RUN(App);
