#define CLAY_WINDOW_HEIGHT 1350
#define CLAY_WINDOW_WIDTH 630

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

#include <vector>
#include <memory>

class App
{
public:
    bool is_running = true;
    clay::Window *window;
    std::unique_ptr<gfx::Pipeline> pipeline;
    std::vector<float> vertices = {
        -0.5, -0.5, 0.0,
        0.0, 0.5, 0.0,
        0.5, -0.5, 0.0};
    std::vector<float> colors = {
        1.0, 0.0, 0.0,
        0.0, 1.0, 0.0,
        0.0, 0.0, 1.0};
    gfx::Uniform uniform_proj;
    gfx::VertexArray *vao;
    gfx::Buffer *vbo;
    gfx::Buffer *cbo;

    App(void *app)
    {
        window = clay::Window::create(CLAY_WINDOW_WIDTH, CLAY_WINDOW_HEIGHT, "Clay", app);

        gfx::ShaderModule vertex_shader(gfx::ShaderType::Vertex);
        vertex_shader.set_source(R"(#version 300 es
            layout(location=0) in vec3 inPosition;
            layout(location=1) in vec3 inColor;

            uniform mat4 proj;

            out vec3 fragColor;

            void main() {
                fragColor = inColor;
                gl_Position = proj * vec4(inPosition, 1.0);
            }
        )");
        vertex_shader.compile();

        gfx::ShaderModule fragment_shader(gfx::ShaderType::Fragment);
        fragment_shader.set_source(R"(#version 300 es
            precision mediump float;

            out vec4 outColor;

            in vec3 fragColor;

            void main() {
                outColor = vec4(fragColor, 1.0);
            }
        )");
        fragment_shader.compile();

        pipeline = std::make_unique<gfx::Pipeline>();
        pipeline->attach_shader(vertex_shader);
        pipeline->attach_shader(fragment_shader);
        pipeline->link();

        uniform_proj = pipeline->get_uniform("proj");

        vbo = new gfx::Buffer(gfx::BufferType::Array);
        cbo = new gfx::Buffer(gfx::BufferType::Array);
        vao = new gfx::VertexArray();

        vbo->set_data(vertices.data(), vertices.size() * sizeof(float), gfx::BufferUsage::Static);
        cbo->set_data(colors.data(), colors.size() * sizeof(float), gfx::BufferUsage::Static);
        vao->set_attribute(0, *vbo, 3, gfx::DataType::Float, 0, 0);
        vao->set_attribute(1, *cbo, 3, gfx::DataType::Float, 0, 0);
    }

    void update()
    {
        float aspect = (float)window->height / (float)window->width;
        Mat4 proj = Mat4::ortho(1.0, -1.0, -aspect, aspect, 1.0f, -1.0f);

        pipeline->use();
        uniform_proj.set_mat4(&proj.data[0]);
        vao->bind();
        gfx::draw(3);

        window->update();
        is_running = window->is_open();
    }
};

CLAY_RUN(App);
