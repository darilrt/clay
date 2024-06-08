#include <glad/glad.h>
#include <glad/glad.c>
#include <GL/gl.h>
#include <SDL2/SDL.h>
#include <iostream>

#include "debug.hpp"
#include "gfx.hpp"

namespace gfx
{
    void init()
    {
        if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
        {
            debug::log("Failed to initialize GLAD");
        }
        else
        {
            debug::log("GLAD initialized");
        }
    }

    void clear_color(float r, float g, float b, float a)
    {
        GL_CALL(glClearColor(r, g, b, a));
    }

    void clear()
    {
        GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
    }

    void draw(size_t vertex_count, size_t instance_count, size_t first_vertex, size_t first_instance, PrimitiveType primitive_type)
    {
        const auto _type = (GLenum)primitive_type;
        if (instance_count <= 1)
        {
            GL_CALL(glDrawArrays(_type, first_vertex, vertex_count));
        }
        else
        {
            GL_CALL(glDrawArraysInstanced(_type, first_vertex, vertex_count, instance_count));
        }
    }

    void draw_instanced(size_t vertex_count, size_t instance_count, size_t first_vertex, size_t first_instance, PrimitiveType primitive_type)
    {
        GL_CALL(glDrawArraysInstanced((GLenum)primitive_type, first_vertex, vertex_count, instance_count));
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
            debug::log("Shader compilation failed: {}", std::string(infoLog));
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
            debug::log("Pipeline linking failed: {}", std::string(infoLog));
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

    void VertexArray::set_attribute(size_t index, Buffer &buffer, size_t size, DataType type, size_t stride, size_t offset)
    {
        bind();
        buffer.bind();
        GL_CALL(glVertexAttribPointer(index, size, (GLenum)type, GL_FALSE, stride, (void *)offset));
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

    void VertexArray::enable_attribute(size_t index)
    {
        bind();
        GL_CALL(glEnableVertexAttribArray(index));
        unbind();
    }

    void VertexArray::set_attribute_divisor(size_t index, size_t divisor)
    {
        bind();
        GL_CALL(glVertexAttribDivisor(index, divisor));
        unbind();
    }

    Image::Image(int width, int height, TextureFormat format)
    {
        this->format = format;

        GL_CALL(glGenTextures(1, &id));
        GL_CALL(glBindTexture(GL_TEXTURE_2D, id));
        GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, (GLenum)format, width, height, 0, (GLenum)format, GL_UNSIGNED_BYTE, NULL));
        GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
    }

    Image::~Image()
    {
        GL_CALL(glDeleteTextures(1, &id));
    }

    void Image::set_data(const void *data, size_t width, size_t height, size_t channels)
    {
        GL_CALL(glBindTexture(GL_TEXTURE_2D, id));
        GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, (GLenum)format, width, height, 0, (GLenum)format, GL_UNSIGNED_BYTE, data));
        GL_CALL(glGenerateMipmap(GL_TEXTURE_2D));
        GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
    }

    void Image::bind(int slot)
    {
        GL_CALL(glActiveTexture(GL_TEXTURE0 + slot));
        GL_CALL(glBindTexture(GL_TEXTURE_2D, id));
    }

    void Image::unbind(int slot)
    {
        GL_CALL(glActiveTexture(GL_TEXTURE0 + slot));
        GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
    }

    void Image::_apply_sampler(Sampler &sampler)
    {
        GL_CALL(glBindTexture(GL_TEXTURE_2D, id));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLenum)sampler.min_filter));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLenum)sampler.mag_filter));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (GLenum)sampler.wrap_s));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (GLenum)sampler.wrap_t));
        GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
    }

    Framebuffer::Framebuffer(int width, int height)
    {
        _width = width;
        _height = height;
        GL_CALL(glGenFramebuffers(1, &id));
    }

    Framebuffer::~Framebuffer()
    {
        GL_CALL(glDeleteFramebuffers(1, &id));
    }

    void Framebuffer::bind()
    {
        GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, id));
    }

    void Framebuffer::unbind()
    {
        GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    }

    void Framebuffer::attach(AttachmentType attachment, Image &image)
    {
        GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, id));
        GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, (GLenum)attachment, GL_TEXTURE_2D, image.id, 0));
        GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    }
}

void CheckOpenGLError(const char *stmt, const char *fname, int line)
{
    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
    {
        std::cerr << "OpenGL error: " << err << " at " << fname << ":" << line << " - for " << stmt << std::endl;
        exit(1);
    }
}
