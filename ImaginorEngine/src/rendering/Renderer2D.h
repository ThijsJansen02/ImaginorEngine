#pragma once
#include <ImaginorPlatform/src/platform.h>
#include <core.h>
#include <ImaginorMath/src/LinearMath/linearmath.h>
#include <ImaginorEngine\src\memory.h>

#define MAX_TEXTURES 32 

namespace IME::Renderer2D {

    struct BatchVertex;

    struct SceneData {
        mat4 viewprojection;
    };

    struct stats {
        uint32 drawcalls;
        uint32 quadcount;
    };

    struct BatchRenderer2DData {
        
        sizeptr vertexsize;
        uint32 vertexcount;
        uint32 indexcount;

        sizeptr vertexbuffersize;
        sizeptr indexbuffersize;

        gl_id shader;

        uint32 vertexoffset;

        gl_id scenebuffer;

        gl_id vertexbuffer;
        gl_id renderbuffer;
        BatchVertex* localbuffer;

        gl_id textures[MAX_TEXTURES];
        uint32 storedtextures;

        SceneData scene;

        Renderer2D::stats stats;

        RenderCommands rendercommands;
    };

    void setup(BatchRenderer2DData* data, sizeptr maxquadcount, const PlatformInterface& platform);
    void setBatchRendererData(BatchRenderer2DData* data);
    void setShader(gl_id shaderid);
    gl_id loadBatchShader(const char* vertexpath, const char* fragmentpath, const PlatformInterface& platform);
    void beginScene(mat4 viewprojection);
    void endScene();
    uint32 pushTexture(gl_id texture);
    void drawQuad(vec3f pos, vec2f size, vec4f color, vec2f* texcoords, uint32 textureindex);
    void drawQuad(vec2f pos, vec2f size, vec4f color);
    void drawQuad(const mat4& transform, vec4f color);
    void drawQuadTL(vec2f pos, vec2f size, vec4f color);
    void drawTexturedQuad(const mat4& transform, vec4f colortint, vec2f* texcoords);
    void drawTexturedQuad(vec2f pos, vec2f size, vec4f colortint, vec2f* texcoords);
    void drawTexturedQuad(vec3f pos, vec2f size, vec4f colortint, vec2f* texcoords);
    void drawTexturedQuad(vec2f pos, vec2f size, vec4f colortint);
    void drawTexturedQuad(vec2f pos, vec2f size);
    void flush();
    void flushAndReset();
    void flushIntermediate();

    Renderer2D::stats getStats();
    void resetStats();

}