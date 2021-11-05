#pragma once

namespace IME {

    struct Canvas;
    struct EventQueue;

    enum operation : int32 {
        IME_DEPTH_TEST,
        IME_STENCIL_TEST
    };

    enum clearbuffer : int32 {
        IME_COLOR_BUFFER    = 1 << 0,
        IME_DEPTH_BUFFER    = 1 << 1,
        IME_STENCIL_BUFFER  = 1 << 2
    };

    enum gsdatatype : int32 {
        IME_NONE,
        IME_UINT8,   
        IME_UINT16,  
        IME_UINT32,
        IME_INT8,    
        IME_INT16,   
        IME_INT32,
        IME_FLOAT,   
        IME_FLOAT2,  
        IME_FLOAT3,  
        IME_FLOAT4,
        IME_DOUBLE,  
        IME_DOUVLE2, 
        IME_DOUBLE3, 
        IME_DOUBLE4,
        IME_MAT2,
        IME_MAT3,
        IME_MAT4
    };

    enum gsdrawmode : int32 {
        IME_LINES,
        IME_LINE_STRIP,
        IME_POINTS,
        IME_TRIANGLES
    };

    enum gsbufferusage : int32 {
        IME_DYNAMIC_DRAW,
        IME_STATIC_DRAW
    };

    enum gstextureformat : int32 {
        IME_R, IME_G, IME_B, IME_A,
        IME_RG,
        IME_RGB,
        IME_RGBA,
        IME_DEPTH_COMPONENT16,
        IME_DEPTH_COMPONENT24,
        IME_DEPTH_COMPONENT32,
        IME_DEPTH24_STENCIL8
    };

    /*
    enum gscolorencoding {
        IME_R8, IME_G8, IME_B8, IME_A8,
        IME_RGB8, IME_RGBA8,
        IME_BGR8, IME_ABGR8
    };*/

    enum gsshaderprogramtype  : int32 {
        IME_FRAGMENT,
        IME_VERTEX
    };

    enum gssbuffertypes : int32 {
        IME_VERTEX_BUFFER,
        IME_INDEX_BUFFER,
        IME_UNIFORM_BUFFER
    };

    enum gstexturewrap : int32 {
        IME_REPEAT,
        IME_MIRRORED_REPEAT,
        IME_CLAMP_TO_EDGE,
        IME_CLAMP_TO_BORDER
    };

    enum gstexturefiltering : int32 {
        IME_NEAREST,
        IME_LINEAR,
        IME_NEAREST_MIPMAP_NEAREST,
        IME_NEAREST_MIPMAP_LINEAR,
        IME_LINEAR_MIPMAP_NEAREST,
        IME_LINEAR_MIPMAP_LINEAR
    };

    enum gsframebufferattachmenttype : int32 {
        IME_COLOR_ATTACHMENT0,
        IME_COLOR_ATTACHMENT1,
        IME_COLOR_ATTACHMENT2,
        IME_COLOR_ATTACHMENT3,
        IME_COLOR_ATTACHMENT4,
        IME_COLOR_ATTACHMENT5,
        IME_COLOR_ATTACHMENT6,
        IME_COLOR_ATTACHMENT7,
        IME_COLOR_ATTACHMENT8,
        IME_COLOR_ATTACHMENT9,
        IME_COLOR_ATTACHMENT10,
        IME_COLOR_ATTACHMENT11,
        IME_COLOR_ATTACHMENT12,
        IME_COLOR_ATTACHMENT13,
        IME_COLOR_ATTACHMENT14,
        IME_COLOR_ATTACHMENT15,
        IME_DEPTH_ATTACHMENT,
        IME_STENCIL_ATTACHMENT,
        IME_DEPTH_STENCIL_ATTACHMENT
    };

    struct TextureProperties {
        int32 width;
        int32 height;
        gstextureformat format;

        bool32 generatemipmaps;

        gstexturewrap S;
        gstexturewrap T;

        gstexturefiltering magfilter;
        gstexturefiltering minfilter;
    };

    struct LayoutChunk {
        IME::gsdatatype type;
        IME::uint32 layoutlocation;
        IME::bool32 normalized;
    };

    struct BufferLayout {
        LayoutChunk* data;
        sizeptr count;
    };

    typedef IME::uint32 gl_id;

    #define IME_GLAPI_INIT(name) void name(IME::Canvas* canvas, IME::byte* memory, IME::sizeptr graphicsmemorysize, IME::EventQueue* eventqueue)

    //rbos
    #define IME_GLAPI_RBO_CREATE(name) gl_id name()
    #define IME_GLAPI_RBO_BIND(name) void name(gl_id id)
    #define IME_GLAPI_RBO_ADD_BUFFER(name) gl_id name(byte* data, sizeptr size, BufferLayout layout, gsbufferusage usage)
    #define IME_GLAPI_RBO_SET_INDEXBUFFER(name) gl_id name(byte* data, sizeptr size, gsdatatype indextype, gsbufferusage usage)

    //vbos
    #define IME_GLAPI_VBO_BIND(name) void name(gl_id id)
    #define IME_GLAPI_VBO_BUFFER_SUB_DATA(name) void name(byte* data, sizeptr size, sizeptr offset)

    //uniform buffer objects
    #define IME_GLAPI_UBO_CREATE(name) gl_id name(byte* data, sizeptr size, gsbufferusage usage)
    #define IME_GLAPI_UBO_BIND(name) void name(gl_id id, uint32 bindingpoint, sizeptr size, sizeptr offset)
    #define IME_GLAPI_UBO_BUFFER_DATA(name) void name(byte* data, sizeptr size, gsbufferusage usage)
    #define IME_GLAPI_UBO_BUFFER_SUB_DATA(name) void name(byte* data, sizeptr size, sizeptr offset)

    //fbos
    #define IME_GLAPI_FBO_CREATE(name) gl_id name()
    #define IME_GLAPI_FBO_BIND(name) void name(gl_id id)
    #define IME_GLAPI_FBO_CREATE_TEXTURE_ATTACHMENT(name) gl_id name(gsframebufferattachmenttype type, TextureProperties properties)

    //drawing
    #define IME_GLAPI_DRAW_ARRAY(name) void name(gsdrawmode mode, sizeptr count)
    #define IME_GLAPI_DRAW_INDEXED(name) void name(gsdrawmode mode, sizeptr count)

    //shaders
    #define IME_GLAPI_SHADER_CREATE(name) gl_id name()
    #define IME_GLAPI_SHADER_ADD_PROGRAM(name) void name(gsshaderprogramtype type, const char* src)
    #define IME_GLAPI_SHADER_BIND(name) void name(gl_id id)
    #define IME_GLAPI_SHADER_SET_UBO_BINDINGPOINT(name) void name(const char* uniformname, uint32 bindingpoint)
    #define IME_GLAPI_SHADER_SET_TEXTURE_BINDING(name) void name(const char* uniformname, int32 bindingpoint)
    #define IME_GLAPI_SHADER_COMPILE(name) void name()

    //setup
    #define IME_GLAPI_SET_VIEWPORT(name) void name(int32 x, int32 y, int32 width, int32 height)
    #define IME_GLAPI_CLEAR(name) void name(int buffers)

    #define IME_GLAPI_ENABLE(name) void name(operation enable)
    #define IME_GLAPI_DISABLE(name) void name(operation disable)

    //textures
    #define IME_GLAPI_TEXTURE_CREATE(name) gl_id name(TextureProperties properties, byte* src, gstextureformat srcformat, gsdatatype srcdatatype)
    #define IME_GLAPI_TEXTURE_BIND(name) void name(gl_id id, uint32 bindingpoint)

    typedef IME_GLAPI_INIT(glapi_init);

    //setup
    typedef IME_GLAPI_SET_VIEWPORT(glapi_set_viewport);
    typedef IME_GLAPI_CLEAR(glapi_clear);

    typedef IME_GLAPI_ENABLE(glapi_enable);
    typedef IME_GLAPI_DISABLE(glapi_disable);

    //rbos
    typedef IME_GLAPI_RBO_CREATE(glapi_rbo_create);
    typedef IME_GLAPI_RBO_BIND(glapi_rbo_bind);
    typedef IME_GLAPI_RBO_SET_INDEXBUFFER(glapi_rbo_set_indexbuffer);
    typedef IME_GLAPI_RBO_ADD_BUFFER(glapi_rbo_add_buffer);

    //vbos
    typedef IME_GLAPI_VBO_BIND(glapi_vbo_bind);
    typedef IME_GLAPI_VBO_BUFFER_SUB_DATA(glapi_vbo_buffer_sub_data);

    //uniform buffer objects
    typedef IME_GLAPI_UBO_CREATE(glapi_ubo_create);
    typedef IME_GLAPI_UBO_BIND(glapi_ubo_bind);
    typedef IME_GLAPI_UBO_BUFFER_DATA(glapi_ubo_buffer_data);
    typedef IME_GLAPI_UBO_BUFFER_SUB_DATA(glapi_ubo_buffer_sub_data);

    //fbos
    typedef IME_GLAPI_FBO_CREATE(glapi_fbo_create);
    typedef IME_GLAPI_FBO_BIND(glapi_fbo_bind);
    typedef IME_GLAPI_FBO_CREATE_TEXTURE_ATTACHMENT(glapi_fbo_create_texture_attachment);

    //shaders
    typedef IME_GLAPI_SHADER_CREATE(glapi_shader_create);
    typedef IME_GLAPI_SHADER_ADD_PROGRAM(glapi_shader_add_program);
    typedef IME_GLAPI_SHADER_BIND(glapi_shader_bind);
    typedef IME_GLAPI_SHADER_SET_UBO_BINDINGPOINT(glapi_shader_set_ubo_bindingpoint);
    typedef IME_GLAPI_SHADER_SET_TEXTURE_BINDING(glapi_shader_set_texture_binding);
    typedef IME_GLAPI_SHADER_COMPILE(glapi_shader_compile);

    //textures
    typedef IME_GLAPI_TEXTURE_CREATE(glapi_texture_create);
    typedef IME_GLAPI_TEXTURE_BIND(glapi_texture_bind);

    //drawing
    typedef IME_GLAPI_DRAW_ARRAY(glapi_draw_array);
    typedef IME_GLAPI_DRAW_INDEXED(glapi_draw_indexed);

}