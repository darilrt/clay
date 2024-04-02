#include "mesh.hpp"

Mesh::Mesh(std::vector<float> vertices, std::vector<float> uv)
{
    vbo = new gfx::Buffer(gfx::BufferType::Array);
    tbo = new gfx::Buffer(gfx::BufferType::Array);
    vao = new gfx::VertexArray();
    vertex_count = vertices.size() / 3;

    vbo->set_data(vertices.data(), vertices.size() * sizeof(float), gfx::BufferUsage::Static);
    tbo->set_data(uv.data(), uv.size() * sizeof(float), gfx::BufferUsage::Static);
    vao->set_attribute(0, *vbo, 3, gfx::DataType::Float, 0, 0);
    vao->set_attribute(1, *tbo, 2, gfx::DataType::Float, 0, 0);
}

void Mesh::draw()
{
    vao->bind();
    gfx::draw(vertex_count);
}
