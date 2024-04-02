#pragma once

#include "clay_math.hpp"
#include "clay_gfx.hpp"
#include "clay.hpp"

#include <vector>

class Mesh
{
public:
    gfx::VertexArray *vao;
    gfx::Buffer *vbo;
    gfx::Buffer *tbo;
    uint32_t vertex_count;

    Mesh(std::vector<float> vertices, std::vector<float> uv);

    void draw();
};