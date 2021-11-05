#include "memory.h"
#include <ImaginorPlatform/src/imegs.h>
#include "primitves.h"
#include "rendering.h"
#include <ImaginorMath/src/LinearMath/vec4.h>

namespace IME::GL {

    internal byte* 
    allocator(sizeptr size);

    internal void 
    deallocator(sizeptr size, byte* data);

    struct VertexBufferObject {

        VBOLayout layout;
        uint32 vertexsize;

        sizeptr size;
        byte* data;
    };

    struct IndexBufferObject {

        byte* data;
        sizeptr size;
        gsdatatype datatype;
    };

    struct GLState {

        PrimitiveSet<VertexBufferObject, 128> vbos;
        PrimitiveSet<IndexBufferObject, 128> ibos;
        
        Canvas* canvas;
        MemoryArena graphicsmemory;
        MemoryPool mainmemory;

        IME::mat3 torastercoordinates;

        gl_id boundvbo;
    }

    global_var GLState glstate;


    global_var sizeptr datatypesizes[18] = {
        0,
        sizeof(uint8),
        sizeof(uint16),
        sizeof(uint32),
        sizeof(int8),
        sizeof(int16),
        sizeof(int32),
        sizeof(real32),
        2 * sizeof(real32),
        3 * sizeof(real32),
        4 * sizeof(real32),
        sizeof(real64),
        2 * sizeof(real64),
        3 * sizeof(real64),
        4 * sizeof(real64),
        2 * 2 * sizeof(real32),
        3 * 3 * sizeof(real32),
        4 * 4 * sizeof(real32)
    };
 
    sizeptr getDataTypeSize(gsdatatype datatype) {
        return datatypesizes[(uint32)datatype];
    }

    internal byte* 
    allocator(sizeptr size) {
        return (byte*)allocateMemory_(&glstate.mainmemory, size);
    }
 
    internal void 
    deallocator(sizeptr size, byte* data) {
        deallocateMemory_(&glstate.mainmemory, data, size);
    }

    extern "C" IME_GLAPI_INIT(ime_glapi_init) { //void ime_glapi_init(IME::Canvas canvas, IME::uint8* memory, IME::sizeptr graphicsmemorysize)

        glstate.canvas = canvas;

        glstate.torastercoordinates = transformMat3(vec2<real32>{ canvas->width / 2.0f, -(canvas->height / 2.0f) }, vec2<real32>{ canvas->width / 2.0f, canvas->height / 2.0f });

        glstate.graphicsmemory.base = memory;
        glstate.graphicsmemory.size = graphicsmemorysize;
        glstate.graphicsmemory.used = 0;

        glstate.mainmemory = createMemoryPool(&glstate.graphicsmemory, glstate.graphicsmemory.size - glstate.graphicsmemory.used, 512);
    }

    extern "C" IME_GLAPI_CREATE_IBO(ime_glapi_create_ibo) { //gl_id ime_glapi_create_ibo(byte* data, sizeptr count, gsdatatype type)

        IndexBufferObject result = {0};

        result.size = count * getDataTypeSize(type);
        result.data = allocator(result.size);
        result.datatype = type;

        IME::copy(data, result.data, result.size);
        return addNewPrimitive(&glstate.ibos, result);
    }


    extern "C" IME_GLAPI_CREATE_VBO(ime_glapi_create_vbo) { //gl_id ime_glapi_create_vbo(void* data, sizeptr size, VBOLayout layout)
        VertexBufferObject result = {0};
        result.size = size;
        result.data = allocator(result.size);
        IME::copy(data, result.data, result.size);

        result.layout.data = (VBOLayoutChunk*)allocator(layout.count * sizeof(VBOLayoutChunk));
        result.layout.count = layout.count;
        for(uint32 i = 0; i < result.layout.count; i++) {
            result.vertexsize += getDataTypeSize(layout.data[i].type);
            result.layout.data[i] = layout.data[i];
        }
        return addNewPrimitive(&glstate.vbos, result);
    }

    extern "C" IME_GLAPI_BIND_VBO(ime_glapi_bind_vbo) { //void ime_glapi_bind_vbo(gl_id id)
        glstate.boundvbo = id;
    }


    extern "C" IME_GLAPI_DRAW_ARRAY(ime_glapi_draw_array) { //void ime_glapi_draw_triangles(sizeptr count)

        VertexBufferObject boundvbo = glstate.vbos.data[glstate.boundvbo];

        if(count == 0) {
            uint64 vertexcount = boundvbo.size / (boundvbo.vertexsize);
            Vertex* vertices = (Vertex*)boundvbo.data;
            for(uint64 i = 0; i < vertexcount - 2; i += 3) {
                drawTriangle(
                            Vertex{vec2ToVec3(glstate.torastercoordinates * vec2f{vertices[i].pos.x, vertices[i].pos.y}), vertices[i].color}, 
                            Vertex{vec2ToVec3(glstate.torastercoordinates * vec2f{vertices[i + 1].pos.x, vertices[i+ 1].pos.y}), vertices[i + 1].color}, 
                            Vertex{vec2ToVec3(glstate.torastercoordinates * vec2f{vertices[i + 2].pos.x, vertices[i + 2].pos.y}), vertices[i + 2].color}, 
                            * glstate.canvas
                        );
            }
        }
        else {
            uint64 vertexcount = count;
            Vertex* vertices = (Vertex*)boundvbo.data;
            for (uint64 i = 0; i < vertexcount - 2; i += 3) {
                drawTriangle(
                            Vertex{vec2ToVec3<real32>(glstate.torastercoordinates * vec2f{vertices[i].pos.x, vertices[i].pos.y}), vertices[i].color}, 
                            Vertex{vec2ToVec3<real32>(glstate.torastercoordinates * vec2f{vertices[i + 1].pos.x, vertices[i+ 1].pos.y}), vertices[i + 1].color}, 
                            Vertex{vec2ToVec3<real32>(glstate.torastercoordinates * vec2f{vertices[i + 2].pos.x, vertices[i + 2].pos.y}), vertices[i + 2].color}, 
                            * glstate.canvas
                        );
            }
        }

    }
}