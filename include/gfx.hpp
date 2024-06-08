#pragma once

#include <cstdint>
#include <string>

typedef uint32_t glid;
typedef void *id;

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
    enum class PrimitiveType : uint32_t
    {
        Points = 0x0000,
        Lines = 0x0001,
        LineLoop = 0x0002,
        LineStrip = 0x0003,
        Triangles = 0x0004,
        TriangleStrip = 0x0005,
        TriangleFan = 0x0006
    };

    void init();

    void clear_color(float r, float g, float b, float a); // Set the clear color

    void clear();

    void draw(size_t vertex_count, size_t instance_count = 1, size_t first_vertex = 0, size_t first_instance = 0, PrimitiveType primitive_type = PrimitiveType::Triangles);

    void draw_instanced(size_t vertex_count, size_t instance_count = 1, size_t first_vertex = 0, size_t first_instance = 0, PrimitiveType primitive_type = PrimitiveType::Triangles);

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
        StaticDraw = 0x88E4,
        DynamicDraw = 0x88E8,
        StreamDraw = 0x88E0,
        StaticRead = 0x88E5,
        DynamicRead = 0x88E9,
        StreamRead = 0x88E1,
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
        void enable_attribute(size_t index);
        void set_attribute_divisor(size_t index, size_t divisor);
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