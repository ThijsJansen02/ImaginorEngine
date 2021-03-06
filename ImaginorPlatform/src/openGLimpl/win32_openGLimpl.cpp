#include <core.h>
#include <glcorearb.h>
#include <glext.h>
#include <GL/GL.h>
#include "win32_openGLimpl.h"
#include <ImaginorPlatform/src/imegs.h>
#include "primitves.h"
#include "memory.h"
#include <datastructures/lists/arrays/ArrayList.h>
#include "conversion.h"
#include <ImaginorPlatform/src/EventQueue.h>
#include <stdio.h>
#include <ImaginorPlatform/src/platform.h>

internal thread_local PFNGLCHECKFRAMEBUFFERSTATUSPROC glCheckFramebufferStatus_;
internal thread_local PFNGLGENVERTEXARRAYSPROC glGenVertexArrays_;
internal thread_local PFNGLBINDVERTEXARRAYPROC glBindVertexArray_;
internal thread_local PFNGLBINDBUFFERPROC glBindBuffer_;
internal thread_local PFNGLGENBUFFERSPROC glGenBuffers_;
internal thread_local PFNGLBUFFERDATAPROC glBufferData_;
internal thread_local PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray_;
internal thread_local PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer_;
internal thread_local PFNGLBUFFERSUBDATAPROC glBufferSubData_;
internal thread_local PFNGLGETUNIFORMBLOCKINDEXPROC glGetUniformBlockIndex_;
internal thread_local PFNGLUNIFORMBLOCKBINDINGPROC glUniformBlockBinding_;
internal thread_local PFNGLUNIFORM1IPROC glUniform1i_;
internal thread_local PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation_;
internal thread_local PFNGLCREATESHADERPROC glCreateShader_;
internal thread_local PFNGLCREATEPROGRAMPROC glCreateProgram_;
internal thread_local PFNGLGENRENDERBUFFERSPROC glGenRenderbuffers_;
internal thread_local PFNGLRENDERBUFFERSTORAGEPROC glRenderbufferStorage_;
internal thread_local PFNGLBINDRENDERBUFFERPROC glBindRenderbuffer_;
internal thread_local PFNGLFRAMEBUFFERRENDERBUFFERPROC glFramebufferRenderbuffer_;

internal thread_local PFNGLUSEPROGRAMPROC glUseProgram_;
internal thread_local PFNGLSHADERSOURCEPROC glShaderSource_;
internal thread_local PFNGLCOMPILESHADERPROC glCompileShader_;
internal thread_local PFNGLGETSHADERIVPROC glGetShaderiv_;
internal thread_local PFNGLATTACHSHADERPROC glAttachShader_;
internal thread_local PFNGLLINKPROGRAMPROC glLinkProgram_;
internal thread_local PFNGLGETPROGRAMIVPROC glGetProgramiv_;
internal thread_local PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog_;
internal thread_local PFNGLBINDBUFFERBASEPROC glBindBufferBase_;
internal thread_local PFNGLBINDBUFFERRANGEPROC glBindBufferRange_;
internal thread_local PFNGLGENERATEMIPMAPPROC glGenerateMipmap_;
internal thread_local PFNGLACTIVETEXTUREPROC glActiveTexture_;
internal thread_local PFNGLCREATEFRAMEBUFFERSPROC glCreateFramebuffers_;
internal thread_local PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer_;
internal thread_local PFNGLTEXSTORAGE2DPROC glTexStorage2D_;
internal thread_local PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D_;
internal thread_local PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog_;
internal thread_local PFNGLCLEARTEXIMAGEPROC glClearTexImage_;
internal thread_local PFNGLDRAWBUFFERSPROC glDrawBuffers_;

internal thread_local PFNGLDEBUGMESSAGECALLBACKPROC glDebugMessageCallback_;

void
openGLMessageCallback( GLenum source,
            GLenum type,
            GLuint id,
            GLenum severity,
            GLsizei length,
            const GLchar* message,
            const void* userParam);

namespace IME::OpenGL {

    void win32_loadGLextensions() {

        glGenVertexArrays_ = (PFNGLGENVERTEXARRAYSPROC)wglGetProcAddress("glGenVertexArrays");
        glBindVertexArray_ = (PFNGLBINDVERTEXARRAYPROC)wglGetProcAddress("glBindVertexArray");
        glBindBuffer_ = (PFNGLBINDBUFFERPROC)wglGetProcAddress("glBindBuffer");
        glGenBuffers_ = (PFNGLGENBUFFERSPROC)wglGetProcAddress("glGenBuffers");
        glBufferData_ = (PFNGLBUFFERDATAPROC)wglGetProcAddress("glBufferData");
        glEnableVertexAttribArray_ = (PFNGLENABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glEnableVertexAttribArray");
        glVertexAttribPointer_ = (PFNGLVERTEXATTRIBPOINTERPROC)wglGetProcAddress("glVertexAttribPointer");
        glBufferSubData_ = (PFNGLBUFFERSUBDATAPROC)wglGetProcAddress("glBufferSubData");
        glGetUniformBlockIndex_ = (PFNGLGETUNIFORMBLOCKINDEXPROC)wglGetProcAddress("glGetUniformBlockIndex");
        glUniformBlockBinding_ = (PFNGLUNIFORMBLOCKBINDINGPROC)wglGetProcAddress("glUniformBlockBinding");
        glUniform1i_ = (PFNGLUNIFORM1IPROC)wglGetProcAddress("glUniform1i");
        glGetUniformLocation_ = (PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation");
        glCreateShader_ = (PFNGLCREATESHADERPROC) wglGetProcAddress("glCreateShader");
        glCreateProgram_ = (PFNGLCREATEPROGRAMPROC) wglGetProcAddress("glCreateProgram");
        glGenRenderbuffers_ = (PFNGLGENRENDERBUFFERSPROC) wglGetProcAddress("glGenRenderbuffers");
        glRenderbufferStorage_ = (PFNGLRENDERBUFFERSTORAGEPROC) wglGetProcAddress("glRenderbufferStorage");
        glBindRenderbuffer_ = (PFNGLBINDRENDERBUFFERPROC) wglGetProcAddress("glBindRenderbuffer");
        glFramebufferRenderbuffer_ = (PFNGLFRAMEBUFFERRENDERBUFFERPROC) wglGetProcAddress("glFramebufferRenderbuffer");

        glUseProgram_ = (PFNGLUSEPROGRAMPROC)wglGetProcAddress("glUseProgram");
        glShaderSource_ = (PFNGLSHADERSOURCEPROC)wglGetProcAddress("glShaderSource");
        glCompileShader_ = (PFNGLCOMPILESHADERPROC)wglGetProcAddress("glCompileShader");
        glGetShaderiv_ = (PFNGLGETSHADERIVPROC)wglGetProcAddress("glGetShaderiv");
        glAttachShader_ = (PFNGLATTACHSHADERPROC)wglGetProcAddress("glAttachShader");
        glLinkProgram_ = (PFNGLLINKPROGRAMPROC)wglGetProcAddress("glLinkProgram");
        glGetProgramiv_ = (PFNGLGETPROGRAMIVPROC)wglGetProcAddress("glGetProgramiv");
        glGetProgramInfoLog_ = (PFNGLGETPROGRAMINFOLOGPROC)wglGetProcAddress("glGetProgramInfoLog");
        glBindBufferBase_ = (PFNGLBINDBUFFERBASEPROC )wglGetProcAddress("glBindBufferBase");
        glBindBufferRange_ = (PFNGLBINDBUFFERRANGEPROC)wglGetProcAddress("glBindBufferRange");
        glGenerateMipmap_ = (PFNGLGENERATEMIPMAPPROC)wglGetProcAddress("glGenerateMipmap");
        glActiveTexture_ = (PFNGLACTIVETEXTUREPROC)wglGetProcAddress("glActiveTexture");
        glCreateFramebuffers_ = (PFNGLCREATEFRAMEBUFFERSPROC)wglGetProcAddress("glCreateFramebuffers");
        glBindFramebuffer_ = (PFNGLBINDFRAMEBUFFERPROC)wglGetProcAddress("glBindFramebuffer");
        glTexStorage2D_ = (PFNGLTEXSTORAGE2DPROC)wglGetProcAddress("glTexStorage2D");
        glFramebufferTexture2D_ = (PFNGLFRAMEBUFFERTEXTURE2DPROC)wglGetProcAddress("glFramebufferTexture2D"); 
        glGetShaderInfoLog_ = (PFNGLGETSHADERINFOLOGPROC)wglGetProcAddress("glGetShaderInfoLog");
        glClearTexImage_ = (PFNGLCLEARTEXIMAGEPROC)wglGetProcAddress("glClearTexImage");
        glCheckFramebufferStatus_ = (PFNGLCHECKFRAMEBUFFERSTATUSPROC)wglGetProcAddress("glCheckFramebufferStatus");
        glDrawBuffers_ = (PFNGLDRAWBUFFERSPROC)wglGetProcAddress("glDrawBuffers");

        glDebugMessageCallback_ = (PFNGLDEBUGMESSAGECALLBACKPROC)wglGetProcAddress("glDebugMessageCallback");

        int32 offset;
        glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &offset);
    }

    template<typename T>
    using ArrayList = Data::ArrayList_<T, allocator, deallocator>; 

    struct ShaderProgram {
        GLint program;
        
        GLint vertexshader;
        GLint fragmentshader;
    };

    struct VertexBufferObject {
        GLuint id;
        sizeptr size;
        sizeptr stride;
        BufferLayout layout;
    };

    struct UniformBufferObject {
        GLuint id;
        sizeptr size;
    };

    struct IndexBufferObject {
        GLuint id = 0;
        gsdatatype datatype;
        sizeptr count;
    };

    struct VertexArrayObject {
        GLuint id = 0;

        gl_id ibo;
        ArrayList<gl_id> vbos;
    };

    struct RenderBufferObject {
        GLuint id = 0;
        uint32 width;
        uint32 height;

        gl_id parentfbo;
        gstextureformat format;
    };

    struct Texture {

        GLuint id = 0;
        TextureProperties props;
    };

    struct FrameBufferAttachment {
        gsframebufferattachmenttype attachment;
        bool32 istexture;
        gl_id ptr;
    };

    struct FrameBuffer {
        GLuint id = 0;

        uint32 width;
        uint32 height;

        ArrayList<FrameBufferAttachment> attachments;
    };

    //the state of the graphics API
    struct State {
        OpenGL::MemoryArena memoryarena;
        OpenGL::MemoryPool mainmemory;
        bool32 inited;

        OpenGL::PrimitiveSet<VertexArrayObject, 1024> vaos;
        OpenGL::PrimitiveSet<IndexBufferObject, 1024> ibos;
        OpenGL::PrimitiveSet<VertexBufferObject, 1024> vbos;
        OpenGL::PrimitiveSet<ShaderProgram, 1024> shaders;
        OpenGL::PrimitiveSet<UniformBufferObject, 1024> ubos;
        OpenGL::PrimitiveSet<Texture, 1024> textures;
        OpenGL::PrimitiveSet<FrameBuffer, 1024> fbos;
        OpenGL::PrimitiveSet<RenderBufferObject, 1024> rbos;

        EventQueue* events;

        gl_id boundfbo;
        gl_id boundubo;
        gl_id boundvao;
        gl_id boundvbo;
        gl_id boundibo;
        gl_id boundshader;
        gl_id boundtexture;
        gl_id boundrbo;
    };

    global_var thread_local State glstate = {0};

    global_var IME::int32 openglbufferusegetable[] = {
        GL_DYNAMIC_DRAW,
        GL_STATIC_DRAW  
    };

    int32 getOpenGLBufferUsage(gsbufferusage bufferusage) {
        return openglbufferusegetable[bufferusage];
    }

    extern "C" void ime_glapi_destroy(IME::Canvas* canvas, IME::byte* memory, IME::sizeptr graphicsmemorysize, IME::EventQueue* eventqueue) {
        
        

        memset(memory, 0, 1024);
    }

    extern "C" IME_GLAPI_INIT(ime_glapi_init) { //void ime_glapi_init(IME::Canvas* canvas, IME::byte* memory, IME::sizeptr graphicsmemorysize, IME::EventQueue* eventqueue)

        memset(memory, 0, 1024);

        glstate.memoryarena = createMemoryArena(memory, graphicsmemorysize);
        glstate.mainmemory = createMemoryPool(&glstate.memoryarena, glstate.memoryarena.size, 1024);

        glstate.boundibo = 0;
        glstate.boundvao = 0;
        glstate.boundvbo = 0;
        glstate.boundshader = 0;
        glstate.boundfbo = 0;

        glstate.events = eventqueue;

        glEnable              (GL_DEBUG_OUTPUT);
        glDebugMessageCallback_(openGLMessageCallback, &glstate);

        clearPrimitiveSet(&glstate.vaos);
        VertexArrayObject nullvao;
        nullvao.id = 0;
        nullvao.ibo = 0;
        addNewPrimitive(&glstate.vaos, nullvao);

        clearPrimitiveSet(&glstate.vbos);
        VertexBufferObject nullvbo;
        nullvbo.id = 0;
        addNewPrimitive(&glstate.vbos, nullvbo);

        clearPrimitiveSet(&glstate.ibos);
        IndexBufferObject nullibo;
        nullibo.id = 0;
        addNewPrimitive(&glstate.ibos, nullibo);

        clearPrimitiveSet(&glstate.shaders);
        ShaderProgram nullshader;
        nullshader.program = 0;
        addNewPrimitive(&glstate.shaders, nullshader);

        clearPrimitiveSet(&glstate.ubos);
        UniformBufferObject nullubo;
        nullubo.id = 0;
        nullubo.size = 0;
        addNewPrimitive(&glstate.ubos, nullubo);

        clearPrimitiveSet(&glstate.textures);
        Texture nulltexture;
        nulltexture.id = 0;
        addNewPrimitive(&glstate.textures, nulltexture);

        clearPrimitiveSet(&glstate.fbos);
        FrameBuffer nullfbo;
        nullfbo.id = 0;
        addNewPrimitive(&glstate.fbos, nullfbo);

        clearPrimitiveSet(&glstate.rbos);
        RenderBufferObject nullrbo;
        nullrbo.id = 0;
        addNewPrimitive(&glstate.rbos, nullrbo);
        
        glstate.inited = true;
    }

    extern "C" IME_GLAPI_RBO_CREATE(ime_glapi_create_rbo) { //IME::gl_id IME::OpenGL::ime_glapi_create_rbo_opengl()
        IME_DEBUG_ASSERT_BREAK(glstate.inited, "gl is not yet inited!")

        VertexArrayObject vao;
        vao.ibo = 0;
        vao.vbos.init(0);
        glGenVertexArrays_(1, &vao.id);
        glBindVertexArray_(vao.id);
        glstate.boundvao = addNewPrimitive(&glstate.vaos, vao);
        return glstate.boundvao;
    }

    extern "C" IME_GLAPI_RBO_BIND(ime_glapi_bind_rbo) { //void ime_glapi_bind_vbo(gl_id id)
        IME_DEBUG_ASSERT_BREAK(glstate.inited, "gl is not yet inited!")

        VertexArrayObject* boundvao = &glstate.vaos.data[id];

        glBindVertexArray_(boundvao->id);
        if(boundvao->ibo != 0) {
            glBindBuffer_(GL_ELEMENT_ARRAY_BUFFER, glstate.ibos.data[boundvao->ibo].id);
        }
        glstate.boundvao = id;
    }

    extern "C" IME_GLAPI_RBO_ADD_BUFFER(ime_glapi_add_buffer) { //gl_id ime_glapi_add_buffer_opengl(byte* data, sizeptr size, BufferLayout layout, gsbufferusage usage)
        
        IME_DEBUG_ASSERT_BREAK(glstate.inited, "gl is not yet inited!")
        VertexArrayObject* boundvao = &glstate.vaos.data[glstate.boundvao];

        VertexBufferObject vbo;

        vbo.size = size;
        glGenBuffers_(1, &vbo.id);
        glBindBuffer_(GL_ARRAY_BUFFER, vbo.id);
        glBufferData_(GL_ARRAY_BUFFER, size, data, getOpenGLBufferUsage(usage));

        vbo.layout.count = layout.count;
        vbo.layout.data = (LayoutChunk*)allocator(vbo.layout.count * sizeof(LayoutChunk));

        //copy layout and calculate stride
        vbo.stride = 0;
        for(uint32 i = 0; i < layout.count; i++) {
            vbo.layout.data[i] = layout.data[i];
            vbo.stride += getDataTypeSize(layout.data[i].type);
        }  

        //set vertex layout in opengl
        uint64 offset = 0;
        for(uint32 i = 0; i < vbo.layout.count; i++) {
            LayoutChunk chunk = vbo.layout.data[i];

            glEnableVertexAttribArray_(chunk.layoutlocation);
            glVertexAttribPointer_(
                chunk.layoutlocation,
                getDataTypeComponentCount(chunk.type), 
                getDataType(chunk.type), 
                chunk.normalized? GL_TRUE : GL_FALSE, 
                vbo.stride, 
                (void*)offset
            );

            offset += getDataTypeSize(chunk.type);
        }

        glstate.boundvbo = addNewPrimitive(&glstate.vbos, vbo);
        boundvao->vbos.push_back(glstate.boundvbo);

        return glstate.boundvbo;
    }

    extern "C" IME_GLAPI_RBO_SET_INDEXBUFFER(ime_glapi_rbo_set_indexbuffer) { //gl_id ime_glapi_rbo_set_indexbuffer(byte* data, sizeptr size, gsdatatype indextype, gsbufferusage usage)
        IME_DEBUG_ASSERT_BREAK(glstate.inited, "gl is not yet inited!")
        VertexArrayObject* boundvao = &glstate.vaos.data[glstate.boundvao];

        IndexBufferObject result;
        glGenBuffers_(1, &result.id);
        glBindBuffer_(GL_ELEMENT_ARRAY_BUFFER, result.id);
        glBufferData_(GL_ELEMENT_ARRAY_BUFFER, size, data, getOpenGLBufferUsage(usage));

        result.count = size / getDataTypeSize(indextype);
        result.datatype = indextype;

        glstate.boundibo = addNewPrimitive(&glstate.ibos, result);
        boundvao->ibo = glstate.boundibo;
        return glstate.boundibo;
    }

    extern "C" IME_GLAPI_VBO_BIND(ime_glapi_vbo_bind) { //void ime_glapi_vbo_bind(gl_id id)
        IME_DEBUG_ASSERT_BREAK(glstate.inited, "gl is not yet inited!")

        glstate.boundvbo = id;
        glBindBuffer_(GL_ARRAY_BUFFER, id);
    }

    extern "C" IME_GLAPI_VBO_BUFFER_SUB_DATA(ime_glapi_vbo_buffer_sub_data) { //void ime_glapi_vbo_buffer_sub_data(byte* data, sizeptr size, sizeptr offset)
         IME_DEBUG_ASSERT_BREAK(glstate.inited, "gl is not yet inited!")

        glBufferSubData_(GL_ARRAY_BUFFER, offset, size, data);
    }

    extern "C" IME_GLAPI_DRAW_ARRAY(ime_glapi_draw_array) { //void ime_glapi_draw_array(gsdrawmode mode, sizeptr count)
        IME_DEBUG_ASSERT_BREAK(glstate.inited, "gl is not yet inited!")

        glDrawArrays(getDrawingMode(mode), 0, count);
    }

    extern "C" IME_GLAPI_DRAW_INDEXED(ime_glapi_draw_indexed) { //oid ime_glapi_draw_indexed(gsdrawmode mode, sizeptr count)
        IME_DEBUG_ASSERT_BREAK(glstate.inited, "gl is not yet inited!")

        IndexBufferObject *boundindexbuffer = &glstate.ibos.data[glstate.boundibo];

        glDrawElements(getDrawingMode(mode), count > 0 ? count : boundindexbuffer->count, getDataType(boundindexbuffer->datatype), nullptr);
    }

    extern "C" IME_GLAPI_SHADER_CREATE(ime_glapi_create_shader) { //gl_id ime_glapi_create_shader()
        IME_DEBUG_ASSERT_BREAK(glstate.inited, "gl is not yet inited!")

        ShaderProgram shader;
        shader.program = glCreateProgram_();
        glstate.boundshader = addNewPrimitive(&glstate.shaders, shader);
        return glstate.boundshader;
    }

    extern "C" IME_GLAPI_SHADER_BIND(ime_glapi_bind_shader) { //void ime_glapi_bind_shader(gl_id id)
        IME_DEBUG_ASSERT_BREAK(glstate.inited, "gl is not yet inited!")

        glUseProgram_(glstate.shaders.data[id].program);
        glstate.boundshader = id;
    }

    extern "C" IME_GLAPI_SHADER_SET_UBO_BINDINGPOINT(ime_glapi_shader_set_ubo_bindingpoint) { //void ime_shader_glapi_set_ubo_bindingpoint(const char* uniformname, uint32 bindingpoint)
        IME_DEBUG_ASSERT_BREAK(glstate.inited, "gl is not yet inited!")

        ShaderProgram* shader = &glstate.shaders.data[glstate.boundshader];
        GLint uniformblockindex = glGetUniformBlockIndex_(shader->program, uniformname);
        glUniformBlockBinding_(shader->program, uniformblockindex, bindingpoint);
    }

    extern "C" IME_GLAPI_SHADER_SET_TEXTURE_BINDING(ime_glapi_shader_set_texture_binding) {
        IME_DEBUG_ASSERT_BREAK(glstate.inited, "gl is not yet inited!")
        
        ShaderProgram* shader = &glstate.shaders.data[glstate.boundshader];
        GLint location = glGetUniformLocation_(shader->program, uniformname);
        glUniform1i_(location, bindingpoint); 
    }

    extern "C" IME_GLAPI_SHADER_ADD_PROGRAM(ime_glapi_shader_add_program) { //void ime_glapi_shader_add_program(gsshaderprogramtype type, const char* src)
        IME_DEBUG_ASSERT_BREAK(glstate.inited, "gl is not yet inited!")

        GLint shader;
        int success;
        
        ShaderProgram* boundshader = &glstate.shaders.data[glstate.boundshader];

        //compiling shader
        shader = glCreateShader_(getShaderType(type));
        glShaderSource_(shader, 1, &src, NULL);
        glCompileShader_(shader);
        // print compile errors if any
        glGetShaderiv_(shader, GL_COMPILE_STATUS, &success);
        if(!success)
        {
            char openglinfolog[512];
            if(type == IME_FRAGMENT) {
                glGetShaderInfoLog_(shader, 512, NULL, openglinfolog);
                char errorlog[1024];
                sprintf_s(errorlog, 1024, "shader failed to compile fragment shader\nmessage: %s\nsource: %s", openglinfolog, src);

                Event event;
                event.destinations = IME_CONSOLE;
                event.type = IME_DEBUG_MESSAGE;
                event.param1 = IME_ERROR;
                event.param2 = (uint64)errorlog;
                event.source = IME_RENDER_API;

                glstate.events->output.push_back(copyEvent(event));
            }
            if(type == IME_VERTEX) {
                glGetShaderInfoLog_(shader, 512, NULL, openglinfolog);
                char errorlog[1024];
                sprintf_s(errorlog, 1024, "shader failed to compile vertex shader\nmessage: %s\nsource: %s", openglinfolog, src);

                Event event;
                event.destinations = IME_CONSOLE;
                event.type = IME_DEBUG_MESSAGE;
                event.param1 = IME_ERROR;
                event.param2 = (uint64)errorlog;
                event.source = IME_RENDER_API;

                glstate.events->output.push_back(copyEvent(event));
            }
        };

        glAttachShader_(boundshader->program, shader);
        
        if(type == IME_FRAGMENT) {
            boundshader->fragmentshader = shader;
        }

        if(type == IME_VERTEX) {
            boundshader->vertexshader = shader;
        }
    }

    extern "C" IME_GLAPI_SHADER_COMPILE(ime_glapi_shader_compile) {
        ShaderProgram* boundshader = &glstate.shaders.data[glstate.boundshader];

        int success;
        glLinkProgram_(boundshader->program);
        // print linking errors if any
        glGetProgramiv_(boundshader->program, GL_LINK_STATUS, &success);
        if(!success)
        {
            char openglinfolog[1024];
            glGetProgramInfoLog_(boundshader->program, 1024, NULL, openglinfolog);
            char errorlog[1024];
            sprintf_s(errorlog, 1024, "shader failed to compile\nmessage: %s", openglinfolog);

            Event event;
            event.destinations = IME_CONSOLE;
            event.type = IME_DEBUG_MESSAGE;
            event.param1 = IME_ERROR;
            event.param2 = (uint64)errorlog;
            event.source = IME_RENDER_API;

            glstate.events->output.push_back(copyEvent(event));
        }

    }

    extern "C" IME_GLAPI_UBO_CREATE(ime_glapi_create_ubo) { //gl_id ime_glapi_create_ubo(byte* data, sizeptr size)
        IME_DEBUG_ASSERT_BREAK(glstate.inited, "gl is not yet inited!")
        
        UniformBufferObject ubo;
        ubo.size = size;
        glGenBuffers_(1, &ubo.id);
        glBindBuffer_(GL_UNIFORM_BUFFER, ubo.id);
        glBufferData_(GL_UNIFORM_BUFFER, size, data, getOpenGLBufferUsage(usage));
        glstate.boundubo = addNewPrimitive(&glstate.ubos, ubo);
        glBindBuffer_(GL_UNIFORM_BUFFER, 0);
        return glstate.boundubo;
    }

    extern "C" IME_GLAPI_UBO_BIND(ime_glapi_bind_ubo) { //void ime_glapi_set_ubo_bindingpoint(uint32 bindingpoint, sizeptr size, sizeptr offset)
        IME_DEBUG_ASSERT_BREAK(glstate.inited, "gl is not yet inited!")

        glstate.boundubo = id;
        UniformBufferObject* ubo = &glstate.ubos.data[glstate.boundubo];

        if(size > 0) {
            glBindBufferRange_(GL_UNIFORM_BUFFER,  bindingpoint, ubo->id, offset, size);
        } else {
            glBindBufferBase_(GL_UNIFORM_BUFFER,  bindingpoint, ubo->id);
        }
    }

    extern "C" IME_GLAPI_UBO_BUFFER_DATA(ime_glapi_ubo_buffer_data) { //void ime_ubo_buffer_data(byte* data, sizeptr size, sizeptr offset, gsbufferusage usage)
        IME_DEBUG_ASSERT_BREAK(glstate.inited, "gl is not yet inited!")
        

        UniformBufferObject* ubo = &glstate.ubos.data[glstate.boundubo];

        glBindBuffer_(GL_UNIFORM_BUFFER, ubo->id);
        glBufferData_(GL_UNIFORM_BUFFER, (GLsizeiptr)size, data, getOpenGLBufferUsage(usage));
        glBindBuffer_(GL_UNIFORM_BUFFER, 0);
    }

    extern "C" IME_GLAPI_UBO_BUFFER_SUB_DATA(ime_glapi_ubo_buffer_sub_data) { //void ime_glapi_ubo_buffer_sub_data(byte* data, sizeptr size, sizeptr offset)
        IME_DEBUG_ASSERT_BREAK(glstate.inited, "gl is not yet inited!")
        
        UniformBufferObject* ubo = &glstate.ubos.data[glstate.boundubo];

        glBindBuffer_(GL_UNIFORM_BUFFER, ubo->id);
        glBufferSubData_(GL_UNIFORM_BUFFER, (GLintptr)offset, (GLsizeiptr)size, data);
        glBindBuffer_(GL_UNIFORM_BUFFER, 0);
    }

    extern "C" IME_GLAPI_TEXTURE_CREATE(ime_glapi_texture_create) { //gl_id ime_glapi_texture_create(TextureProperties properties, byte* src, gstextureformat srcformat, gsdatatype srcdatatype)
        IME_DEBUG_ASSERT_BREAK(glstate.inited, "gl is not yet inited!")

        Texture result;
        result.props = properties;

        glGenTextures(1, &result.id);
        glBindTexture(GL_TEXTURE_2D, result.id);

        glTexImage2D(GL_TEXTURE_2D, 0, getInternalColorFormat(properties.format), properties.width, properties.height, 0, getColorFormat(srcformat), getDataType(srcdatatype), src);

        if(properties.generatemipmaps) {
            glGenerateMipmap_(GL_TEXTURE_2D);
        }

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, getTextureFiltering(properties.minfilter));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, getTextureFiltering(properties.magfilter));

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, getTextureWrap(properties.S));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, getTextureWrap(properties.T));

        
        glstate.boundtexture = addNewPrimitive(&glstate.textures, result);
        return glstate.boundtexture;
    }

    extern "C" IME_GLAPI_TEXTURE_RESET(ime_glapi_texture_reset) { //void ime_glapi_texture_resize(uint32 newwidth, uint32 newheight)

        Texture* texture = &glstate.textures.data[glstate.boundtexture];

        glTexImage2D(GL_TEXTURE_2D, 0, getInternalColorFormat(properties.format), properties.width, properties.height, 0, getColorFormat(srcformat), getDataType(srcdatatype), src);

        if(properties.generatemipmaps) {
            glGenerateMipmap_(GL_TEXTURE_2D);
        }

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, getTextureFiltering(properties.minfilter));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, getTextureFiltering(properties.magfilter));

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, getTextureWrap(properties.S));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, getTextureWrap(properties.T));

        texture->props = properties;
    }

    extern "C" IME_GLAPI_TEXTURE_BIND(ime_glapi_texture_bind) { //void ime_glapi_texture_bind(gl_id id, uint32 bindingpoint)
        IME_DEBUG_ASSERT_BREAK(glstate.inited, "gl is not yet inited!")

        glstate.boundtexture = id;
        glActiveTexture_(GL_TEXTURE0 + bindingpoint); // activate the texture unit first before binding texture
        glBindTexture(GL_TEXTURE_2D, glstate.textures.data[id].id);
    }

    extern "C" IME_GLAPI_TEXTURE_CLEAR(ime_glapi_texture_clear) { //void ime_glapi_texture_clear(IME::byte* clearvalue)

        Texture* t = &glstate.textures.data[texture];

        glClearTexImage_(t->id, 0, getColorFormat(t->props.format), getDataTypeForColorFormat(t->props.format), clearvalue);

    }

    extern "C" IME_GLAPI_FBO_CREATE(ime_glapi_fbo_create) { //gl_id ime_glapi_fbo_create(uint32 height, uint32 width)
        IME_DEBUG_ASSERT_BREAK(glstate.inited, "gl is not yet inited!")
        
        FrameBuffer framebuffer;
        glCreateFramebuffers_(1, &framebuffer.id);
        framebuffer.attachments.init(0);
        glBindFramebuffer_(GL_FRAMEBUFFER, framebuffer.id);
        framebuffer.height = height;
        framebuffer.width = width;

        const GLenum buffers[]{ 
            GL_COLOR_ATTACHMENT0, 
            GL_COLOR_ATTACHMENT1, 
            GL_COLOR_ATTACHMENT2, 
            GL_COLOR_ATTACHMENT3, 
            GL_COLOR_ATTACHMENT4 
        };
        glDrawBuffers_(4, buffers );

        glstate.boundfbo = addNewPrimitive(&glstate.fbos, framebuffer);
        return glstate.boundfbo;
    }

    extern "C" IME_GLAPI_FBO_CREATE_TEXTURE_ATTACHMENT(ime_glapi_fbo_create_texture_attachment) { //gl_id ime_glapi_create_texture_attachment(gsframebufferattachmenttype type, TextureProperties properties)
        IME_DEBUG_ASSERT_BREAK(glstate.inited, "gl is not yet inited!")

        Texture result;

        result.props = properties;

        glGenTextures(1, &result.id);
        glBindTexture(GL_TEXTURE_2D, result.id);

        FrameBuffer* framebuffer = &glstate.fbos.data[glstate.boundfbo];
        result.props.height = framebuffer->height;
        result.props.width = framebuffer->width;

        if(type >= IME_COLOR_ATTACHMENT0 && type <= IME_COLOR_ATTACHMENT15) {
            glTexImage2D(
                GL_TEXTURE_2D,                                              //target
                0,                                                        //level
                getInternalColorFormat(properties.format),                          //internal format
                framebuffer->width,                                           //width
                framebuffer->height,                                          //height
                0,                                                    //border
                getColorFormat(properties.format),                         //format 
                GL_UNSIGNED_BYTE,                                            //type
                nullptr                                                     //data
            );
        } else {
            glTexStorage2D_(GL_TEXTURE_2D, 
                1,
                getInternalColorFormat(properties.format), 
                framebuffer->width, 
                framebuffer->height
            );
        }

        if(properties.generatemipmaps) {
            glGenerateMipmap_(GL_TEXTURE_2D);
        }

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, getTextureFiltering(properties.minfilter));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, getTextureFiltering(properties.magfilter));

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, getTextureWrap(properties.S));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, getTextureWrap(properties.T));

        GLenum attachmenttype = getFrameBufferAttachmentType(type);

        glFramebufferTexture2D_(GL_FRAMEBUFFER, attachmenttype, GL_TEXTURE_2D, result.id, 0);

        if(glCheckFramebufferStatus_(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	        IME_DEBUG_BREAK()

        glBindTexture(GL_TEXTURE_2D, 0);

        glstate.boundtexture = addNewPrimitive(&glstate.textures, result);

        FrameBufferAttachment attachement;
        attachement.ptr = glstate.boundtexture;
        attachement.istexture = true;
        attachement.attachment = type;
        framebuffer->attachments.push_back(attachement);

        return glstate.boundtexture;
    }

    extern "C" IME_GLAPI_FBO_BIND(ime_glapi_fbo_bind) {
        IME_DEBUG_ASSERT_BREAK(glstate.inited, "gl is not yet inited!")

        glstate.boundfbo = id;
        glBindFramebuffer_(GL_FRAMEBUFFER, glstate.fbos.data[id].id);
    }

    extern "C" IME_GLAPI_FBO_CREATE_RBO_ATTACHMENT(ime_glapi_fbo_create_rbo_attachment) { //gl_id ime_glapi_fbo_create_rbo_attachment(gstextureformat format)

        RenderBufferObject result;
        FrameBuffer* framebuffer = &glstate.fbos.data[glstate.boundfbo];

        glGenRenderbuffers_(1, &result.id);
        glBindRenderbuffer_(GL_RENDERBUFFER, result.id);
        glRenderbufferStorage_(GL_RENDERBUFFER, getInternalColorFormat(format), framebuffer->width, framebuffer->height);

        if(format >= 7 && format <= 9) {
            glFramebufferRenderbuffer_(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, result.id);
        } else if (format == IME_DEPTH24_STENCIL8) {
            glFramebufferRenderbuffer_(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, result.id);
        } else {
            Event e;
            e.destinations = IME_CONSOLE;
            e.source = IME_RENDER_API;
            e.type = IME_DEBUG_MESSAGE;
            e.param2 = (uint64)"renderbuffer format is not accepted!";
            e.param1 = IME_WARN;
            glstate.events->output.push_back(IME::copyEvent(e));
            return 0;
        }

        glBindRenderbuffer_(GL_RENDERBUFFER, 0);

        result.format = format;
        result.parentfbo = glstate.boundfbo;

        glstate.boundrbo = addNewPrimitive(&glstate.rbos, result);
        
        FrameBufferAttachment attachment;
        attachment.istexture = false;
        attachment.ptr = glstate.boundrbo;


        framebuffer->attachments.push_back(attachment);
        return glstate.boundrbo;
    }

    extern "C" IME_GLAPI_FBO_RESIZE(ime_glapi_fbo_resize) { //bool32 ime_glapi_fbo_resize(uint32 newwidth, uint32 newheight)

        FrameBuffer* framebuffer = &glstate.fbos.data[glstate.boundfbo];
        framebuffer->width = newwidth;
        framebuffer->height = newheight;

        for(FrameBufferAttachment attachment : framebuffer->attachments) {
            if(attachment.istexture) {
                Texture& textureattachment = glstate.textures.data[attachment.ptr];
                glBindTexture(GL_TEXTURE_2D, textureattachment.id);
                glTexImage2D(
                    GL_TEXTURE_2D,                                              //target
                    0,                                                          //level
                    getInternalColorFormat(textureattachment.props.format),                          //internal format
                    framebuffer->width,                                           //width
                    framebuffer->height,                                          //height
                    0,                                                          //border
                    getColorFormat(textureattachment.props.format),                          //format 
                    GL_UNSIGNED_BYTE,                                            //type
                    nullptr                                                     //data
                );
                textureattachment.props.width = newwidth;
                textureattachment.props.height = newheight;
            } else {
                RenderBufferObject& renderbufferattachment = glstate.rbos.data[attachment.ptr];
                renderbufferattachment.width = newwidth;
                renderbufferattachment.height = newheight;

                glBindRenderbuffer_(GL_RENDERBUFFER, renderbufferattachment.id);
                glRenderbufferStorage_(GL_RENDERBUFFER, getColorFormat(renderbufferattachment.format), newwidth, newheight);
            }
        }

        glBindTexture(GL_TEXTURE_2D, glstate.textures.data[glstate.boundtexture].id);
        glBindRenderbuffer_(GL_RENDERBUFFER, glstate.rbos.data[glstate.boundrbo].id);

        return true;
    }

    extern "C" IME_GLAPI_FBO_READ_PIXELS(ime_glapi_fbo_read_pixels) { //gsframebufferattachmenttype attachment, byte* buffer, sizeptr size, sizeptr* readsize, uint32 x, uint32 y
        FrameBuffer* framebuffer = &glstate.fbos.data[glstate.boundfbo];
        
        gl_id texture;
        for (auto fbattachment : framebuffer->attachments) {
            if(fbattachment.istexture) {
                if(fbattachment.attachment == attachment) {
                    texture = fbattachment.ptr;
                }
            }
        }

        if(texture == 0) {
            IME_DEBUG_BREAK()
        }

        gstextureformat format = glstate.textures.data[texture].props.format;

        glReadBuffer(getFrameBufferAttachmentType(attachment));
        glReadPixels(x, y, width, height, getColorFormat(format), getDataTypeForColorFormat(format), buffer);
    }

    extern "C" IME_GLAPI_FBO_DELETE(ime_glapi_fbo_delete) {
        return false;
    }


    extern "C" IME_GLAPI_SET_VIEWPORT(ime_glapi_set_viewport) {
        IME_DEBUG_ASSERT_BREAK(glstate.inited, "gl is not yet inited!")
        
        glViewport(x, y, width, height);
    }

    extern "C" IME_GLAPI_CLEAR(ime_glapi_clear) { //void ime_glapi_clear(clearbuffer buffers)
        IME_DEBUG_ASSERT_BREAK(glstate.inited, "gl is not yet inited!")
        
        glClear(convertClearBuffer((clearbuffer)buffers));
    }

    extern "C" IME_GLAPI_DISABLE(ime_glapi_disable) { //void ime_glapi_disable(operation disable)
        glDisable(convertOperation(disable));
    }

    extern "C" IME_GLAPI_ENABLE(ime_glapi_enable) { //void ime_glapi_enable(operation enable)
        glEnable(convertOperation(enable));
    }

    internal byte* 
    allocator(sizeptr size) {
        
        return (byte*)OpenGL::allocateMemory_(&glstate.mainmemory, size);
    }

    internal void 
    deallocator(sizeptr size, byte* data) {
        deallocateMemory_(&glstate.mainmemory, data, size);
    }
}

void
openGLMessageCallback( GLenum source,
            GLenum type,
            GLuint id,
            GLenum severity,
            GLsizei length,
            const GLchar* message,
            const void* userParam )
{
    
    IME::OpenGL::State *user = (IME::OpenGL::State*)userParam;

    IME::Event event;
    event.type = IME::IME_DEBUG_MESSAGE;
    event.param1 = IME::IME_INFO;
    event.destinations = IME::IME_CONSOLE;
    event.source = IME::IME_RENDER_API;

    char buffer[512];
    char* typestr = "";
    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR:
        typestr = "GL ERROR";
        break;

    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        typestr = "GL UNDEFINED BEHAVIOR";
        break;
    default:
        typestr = "DEFAULT";
        break;
    }

    char* severitystr = "";
    switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH:
        event.param1 = IME::IME_ERROR;
        severitystr = "HIGH";
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        event.param1 = IME::IME_WARN;
        severitystr = "MEDIUM";
        break;
    case GL_DEBUG_SEVERITY_LOW:
        event.param1 = IME::IME_WARN;
        severitystr = "LOW";
        break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        event.param1 = IME::IME_INFO;
        severitystr = "NOTIFICATION";
        //\return;
        break;
    default:
        event.param1 = IME::IME_INFO;
        severitystr = "NOTIFICATION";
        break;
    }

    sprintf_s<512>(buffer, "type = %s\nseverity = %s\nmessage = %s",
        typestr, 
        severitystr, 
        message
    );

    if(severity == GL_DEBUG_SEVERITY_HIGH) {
        OutputDebugString(buffer);
    }
    event.param2 = (IME::uint64)buffer;

    user->events->output.push_back(IME::copyEvent(event));
}   