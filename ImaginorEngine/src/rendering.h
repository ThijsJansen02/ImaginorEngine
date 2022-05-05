#pragma once
#include <datastructures/lists/arrays/ArrayList.h>
#include <ImaginorMath/src/LinearMath/linearmath.h>
#include <ImaginorPlatform/src/platform.h>
#include <datastructures/strings/string.h>
#include "assets.h"

namespace IME::Rendering {

    typedef Data::String<Memory::alloc, Memory::dealloc> RenderString;

    template<typename T>
    using ArrayList = Data::ArrayList_<T, Memory::alloc, Memory::dealloc>;


    struct SimpleQuadCommand {
        
        vec4f color;
        gl_id shader;
        gl_id texture;
        vec3f position;
        vec2f size;
        vec2f texcoords[4];
    };

    struct ComplexQuadCommand {

        vec4f color;
        gl_id shader;
        gl_id texture;
        mat4 transform;
        vec2f texcoords[4];
    };

    struct SimpleTextCommand {

        RenderString text;
        Assets::Font* font;
        vec2f glyphsize;
        vec3f position;
        real32 maxwidth;
        vec4f color;
        real32 linespacing;

    };  

    struct MeshObjectCommand {
        gl_id rbo;
        gl_id shader;
        gl_id texture;
        mat4 transform;
    };

    enum RenderQueueTypes {
        SIMPLE_QUAD,
        COMPLEX_QUAD,
        SIMPLE_TEXT,
        MESH_OBJECT
    };

    typedef ArrayList<ComplexQuadCommand> ComplexQuadRQ;
    typedef ArrayList<SimpleQuadCommand> SimpleQuadRQ;
    typedef ArrayList<MeshObjectCommand> MeshRQ;

    struct RenderQueue {

        bool32 updatescene;
        mat4 projection;
        mat4 view;

        Data::Array<gl_id, 3> uniformbuffers;
        Data::Array<uint32, 3> uniformbufferbindingpoints;

        RenderQueueTypes commandtype;
        gl_id rendertarget;
        uint32 viewwidth;
        uint32 viewheight;
        uint32 viewx;
        uint32 viewy;

        bool32 depthtesting;

        uint32 bufferstoclear;
        vec4f clearcolor;

        byte* data1;
        byte* data2;
        sizeptr count1;
        sizeptr count2;

        bool32 dealloc;
    };

    struct RenderSet {

        Data::ArrayList_<RenderQueue, Memory::alloc, Memory::dealloc> renderqueues;
        Assets::UniformBuffer* buffer;
    };

    RenderSet 
    initRenderSet(sizeptr buffersize, Assets::Library* lib, const PlatformInterface& platform);
    
    void 
    pushQuad(
            const vec3f& pos, const vec2f& size, 
            const vec4f color, 
            gl_id shader, 
            gl_id texture, 
            const vec2f* texcoords, 
            ArrayList<Rendering::SimpleQuadCommand>* queue);

    void 
    pushTexturedQuad(
            const vec3f& pos, const vec2f& size, 
            gl_id shader, 
            gl_id texture, 
            bool flipUVs, 
            ArrayList<Rendering::SimpleQuadCommand>* queue);

    void 
    pushColoredQuad(
            const vec3f& pos, const vec2f& size, 
            gl_id shader, 
            const vec4f color, 
            ArrayList<Rendering::SimpleQuadCommand>* queue);

    void 
    pushQuadTL(const vec3f& pos, const vec2f& size, 
            const vec4f color, 
            gl_id shader, 
            gl_id texture, 
            const vec2f* texcoords, 
            ArrayList<Rendering::SimpleQuadCommand>* queue);
    void
    pushColoredQuadTL(
            const vec3f& pos, const vec2f& size, 
            gl_id shader, 
            const vec4f color, 
            ArrayList<Rendering::SimpleQuadCommand>* queue);

    void 
    pushTexturedQuadTL(
            const vec3f& pos, const vec2f& size, 
            gl_id shader, 
            gl_id texture, 
            bool flipUVs, 
            ArrayList<Rendering::SimpleQuadCommand>* queue);

    void 
    flushRenderSet(const RenderSet& renderset, const PlatformInterface& platform);
}