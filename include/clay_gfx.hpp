//
// This file is part of the "clay" project, and is released under the "MIT License".
// See the LICENSE file for more information.
// Created by Daril Rodriguez on 2024-03-01.
//
// clay_gfx.hpp - A simple graphics library for OpenGL ES and OpenGL (TODO WebGL)
// Version 0.1
//
// Usage:
// #define CLAY_GFX_IMPLEMENTATION
// #include "clay_gfx.hpp"
// #undef CLAY_GFX_IMPLEMENTATION
//
// Features:
// - Shader modules
// - Pipelines
// - Buffers
// - Vertex arrays
// - Images
// - Samplers
// - Framebuffers
//
#pragma once

#include <cstdint>
#include <string>

typedef uint32_t glid;
typedef void *id;

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

    enum class DataType : uint32_t
    {
        Float = 0x1406,
        Int = 0x1404,
        UnsignedInt = 0x1405
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

        void set_attribute(size_t index, Buffer &buffer, size_t size, DataType type, size_t stride, size_t offset);
        void set_index_buffer(Buffer &buffer);
    };

    enum class TextureFormat : uint32_t
    {
        RGB = 6407,
        RGBA = 6408,
        Depth = 0x1902,
    };

    enum class SamplerFilter : uint32_t
    {
        Nearest = 0x2600,
        Linear = 0x2601,
        NearestMipmapNearest = 0x2700,
        LinearMipmapNearest = 0x2701,
        NearestMipmapLinear = 0x2702,
        LinearMipmapLinear = 0x2703,
    };

    enum class SamplerWrap : uint32_t
    {
        Repeat = 0x2901,
        MirroredRepeat = 0x8370,
        ClampToEdge = 0x812F,
        ClampToBorder = 0x812D,
        MirrorClampToEdge = 0x8743,
    };

    class Sampler
    {
    public:
        SamplerFilter min_filter;
        SamplerFilter mag_filter;
        SamplerWrap wrap_s;
        SamplerWrap wrap_t;

        Sampler(SamplerFilter min_filter, SamplerFilter mag_filter, SamplerWrap wrap_s, SamplerWrap wrap_t) : min_filter(min_filter), mag_filter(mag_filter), wrap_s(wrap_s), wrap_t(wrap_t) {}
    };

    class Image
    {
    public:
        glid id = 0;
        TextureFormat format;

        Image(int width, int height, TextureFormat format);
        ~Image();

        void set_data(const void *data, size_t width, size_t height, size_t channels);

        void bind(int slot = 0);
        void unbind(int slot = 0);

        void _apply_sampler(Sampler &sampler);
    };

    enum class AttachmentType : uint32_t
    {
        Color0 = 0x8CE0,
        Color1 = 0x8CE1,
        Color2 = 0x8CE2,
        Color3 = 0x8CE3,
        Color4 = 0x8CE4,
        Color5 = 0x8CE5,
        Color6 = 0x8CE6,
        Color7 = 0x8CE7,
        Depth = 0x8D00,
        Stencil = 0x8D20,
        DepthStencil = 0x821A
    };

    class Framebuffer
    {
    public:
        glid id = 0;
        int _width;
        int _height;

        Framebuffer(int width, int height);
        ~Framebuffer();

        void bind();
        void unbind();

        void attach(AttachmentType attachment, Image &image);
    };

}

#ifdef CLAY_GFX_IMPLEMENTATION

#ifdef CLAY_PLATFORM_ANDROID
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#else
#include <glad/glad.h>
#include <glad/glad.c>
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

#endif // CLAY_GFX_IMPLEMENTATION