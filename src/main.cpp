#define CLAY_WINDOW_HEIGHT 630
#define CLAY_WINDOW_WIDTH 1350

#include <vector>
#include <memory>

void start(void *window);
void loop(void *window);

#ifdef __ANDROID__
#include "main_android.cc"
#else
#include "main_desktop.cc"
#endif

#include "clay_math.hpp"

#ifdef CLAY_PLATFORM_ANDROID
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#else
#include <glad/glad.h>
#include <GL/gl.h>
#endif // CLAY_PLATFORM_ANDROID

std::unique_ptr<gfx::Pipeline> pipeline;
gfx::Attribute vertex, color;
std::vector<float> vertices = {
    -0.5, -0.5, 0.0,
    0.0, 0.5, 0.0,
    0.5, -0.5, 0.0};
std::vector<float> colors = {
    1.0, 0.0, 0.0,
    0.0, 1.0, 0.0,
    0.0, 0.0, 1.0};
std::vector<uint32_t> indices = {
    0, 1, 2};
gfx::Uniform uniform_proj;

void start(void *window)
{
    clay::Window *win = (clay::Window *)window;

    gfx::ShaderModule vertex_shader(gfx::ShaderType::Vertex);
    vertex_shader.set_source(R"(#version 300 es
        in vec3 inPosition;
        in vec3 inColor;

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

    vertex = pipeline->get_attribute("inPosition");
    color = pipeline->get_attribute("inColor");
    uniform_proj = pipeline->get_uniform("proj");
}

void loop(void *window)
{
    clay::Window *win = (clay::Window *)window;

    glClearColor(0.0f, 0.0f, 0.3f, 1.0f);

    pipeline->use();

    static float aspect = (float)win->height / (float)win->width;
    static Mat4 proj = Mat4::ortho(1.0, -1.0, -aspect, aspect, 1.0f, -1.0f);

    uniform_proj.set_mat4(&proj.data[0]);

    vertex.enable();
    vertex.set_pointer(vertices.data(), 3, 3 * sizeof(float));

    color.enable();
    color.set_pointer(colors.data(), 3, 3 * sizeof(float));

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, indices.data());

    vertex.disable();
    color.disable();
}