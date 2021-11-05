#include <core.h>
#include "Renderer2D.h"
#include <ImaginorPlatform/src/imegs.h>
#include <LinearMath/linearmath.h>
#include <stdio.h>

namespace IME::Renderer2D {

    struct BatchVertex {
        vec3f position;
        vec4f color;
        vec2f texcoord;
        real32 index;
    }; 
    LayoutChunk chunks[] = { {IME_FLOAT3, 0, false}, {IME_FLOAT4, 1, false}, {IME_FLOAT2, 2, false}, {IME_FLOAT, 3, false} };
    global_var BufferLayout batchbufferlayout = { chunks, sizeof(chunks) / sizeof(LayoutChunk) };

    global_var BatchRenderer2DData* batchrendererdata_ = nullptr;

    internal void 
    setBatchRendererData(BatchRenderer2DData* data) {
        batchrendererdata_ = data;
    }

    internal void 
    setTextureBindings(const char* formatstring, const RenderCommands& rendercommands) {
        char buffer[256];
        for(uint32 i = 0; i < 32; i++) {
            sprintf_s(buffer, 256, formatstring, i);
            rendercommands.shader_settexturebinding(buffer, i);
        }
    }

    gl_id loadBatchShader(const char* vertexsrcpath, const char* fragmentsrcpath, PlatformInterface* platform) {

        FileBuffer fragmentsrc = platform->io.debug_readfile(fragmentsrcpath, nullptr);
        FileBuffer vertexsrc = platform->io.debug_readfile(vertexsrcpath, nullptr);

        gl_id shader;
        shader = platform->gfx.createshader();
        platform->gfx.shaderaddprogram(IME_VERTEX, (char*)vertexsrc.data);
        platform->gfx.shaderaddprogram(IME_FRAGMENT, (char*)fragmentsrc.data);
        platform->gfx.shader_compile();
        platform->gfx.bindshader(shader);
        setTextureBindings("textures[%d]", platform->gfx);

        platform->io.debug_releasefilememory(&fragmentsrc);
        platform->io.debug_releasefilememory(&vertexsrc);

        return shader;
    }

    void 
    setup(BatchRenderer2DData* data, sizeptr maxquadcount, const RenderCommands& rendercommands, MemoryPool* memory) {
        
        BatchRenderer2DData& batchrendererdata = *batchrendererdata_;

        BatchRenderer2DData result = {};
        result.vertexcount = maxquadcount * 4;
        result.vertexsize = sizeof(BatchVertex);
        result.vertexbuffersize = result.vertexcount * result.vertexsize;

        result.indexcount = (result.vertexcount / 4) * 6;
        result.indexbuffersize = result.indexcount * sizeof(uint32);

        result.localbuffer = (BatchVertex*)allocateMemory_(memory, result.vertexbuffersize);

        result.renderbuffer = rendercommands.rbo_create();
        result.vertexbuffer = rendercommands.rbo_addbuffer(nullptr, result.vertexbuffersize, batchbufferlayout, IME_DYNAMIC_DRAW);

        result.scenebuffer = rendercommands.createubo(nullptr, sizeof(SceneData), IME_DYNAMIC_DRAW);

        result.rendercommands = rendercommands;

        TextureProperties props;
        props.format = IME_RGBA;
        props.height = 1;
        props.width = 1;
        props.magfilter = IME_NEAREST;
        props.minfilter = IME_NEAREST;
        props.S = IME_REPEAT;
        props.T = IME_REPEAT;

        ubyte color[4] = {255, 255, 255, 255};
        result.textures[0] = rendercommands.texture_create(props, (byte*)color, IME_RGBA, IME_UINT8);

        uint32* indeces = (uint32*)allocateMemory_(memory, result.indexbuffersize);

        uint32 vertexcount = 0;
        for(uint32 i = 0; i < result.indexcount; i += 6) {
            
            indeces[i + 0] = vertexcount + 0;
            indeces[i + 1] = vertexcount + 1;
            indeces[i + 2] = vertexcount + 2;
            indeces[i + 3] = vertexcount + 0;
            indeces[i + 4] = vertexcount + 2;
            indeces[i + 5] = vertexcount + 3;

            vertexcount += 4;
        }

        rendercommands.rbo_setindexbuffer((byte*)indeces, result.indexbuffersize, IME_UINT32, IME_STATIC_DRAW);
        deallocateMemory_(memory, (byte*)indeces, result.indexbuffersize);

        *data = result;
        batchrendererdata_ = data;
    }

    void setShader(gl_id shaderid) {
        batchrendererdata_->shader = shaderid;
    }

    void resetStats() {
        batchrendererdata_->stats = {};
    }

    Renderer2D::stats getStats() {
        return batchrendererdata_->stats;
    }

    void 
    beginScene(mat4 viewprojection) {

        BatchRenderer2DData& batchrendererdata = *batchrendererdata_;
        batchrendererdata.scene.viewprojection = viewprojection;

        batchrendererdata.vertexoffset = 0;
        batchrendererdata.storedtextures = 1;
    }

    void 
    endScene() {
        BatchRenderer2DData& batchrendererdata = *batchrendererdata_;
        batchrendererdata.rendercommands.vbo_bind(batchrendererdata.vertexbuffer);
        batchrendererdata.rendercommands.vbo_buffersubdata((byte*)batchrendererdata.localbuffer, batchrendererdata.vertexoffset * batchrendererdata.vertexsize, 0);
    }

    uint32 
    pushTexture(gl_id texture) {
        BatchRenderer2DData& batchrendererdata = *batchrendererdata_;
        batchrendererdata.textures[batchrendererdata.storedtextures++] = texture;
        return batchrendererdata.storedtextures - 1;
    }

    void flushIntermediate();

    inline void 
    drawQuad(const mat4& transform, vec4f color, vec2f* texcoords, uint32 textureindex) {

        BatchRenderer2DData& batchrendererdata = *batchrendererdata_;

        BatchVertex vertex;
        vertex.position = transform * vec3f{-0.5f, -0.5f, 0.0f};
        vertex.color = color;
        vertex.texcoord = texcoords[0];
        vertex.index = (real32)textureindex;

        batchrendererdata.localbuffer[batchrendererdata.vertexoffset + 0] = vertex;

        vertex.position = transform * vec3f{ 0.5f, -0.5f, 0.0f};
        vertex.color = color;
        vertex.texcoord = texcoords[1];
        vertex.index = (real32)textureindex;

        batchrendererdata.localbuffer[batchrendererdata.vertexoffset + 1] = vertex;

        vertex.position = transform * vec3f{ 0.5f, 0.5f, 0.0f};;
        vertex.color = color;
        vertex.texcoord = texcoords[2];
        vertex.index = (real32)textureindex;

        batchrendererdata.localbuffer[batchrendererdata.vertexoffset + 2] = vertex;

        vertex.position = transform * vec3f{ -0.5f, 0.5f, 0.0f};;
        vertex.color = color;
        vertex.texcoord = texcoords[3];
        vertex.index = (real32)textureindex;

        batchrendererdata.localbuffer[batchrendererdata.vertexoffset + 3] = vertex;

        batchrendererdata.vertexoffset += 4;

        if(batchrendererdata.vertexoffset >= batchrendererdata.vertexbuffersize / sizeof(BatchVertex)) {
            flushIntermediate();
        }
    }

    void drawTexturedQuad(const mat4& transform, vec4f colortint, vec2f* texcoords) {
        drawQuad(transform, colortint, texcoords, batchrendererdata_->storedtextures - 1);
    }

    void drawQuad(const mat4& transform, vec4f color) {
        vec2f texcoords[4] = {{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}};
        drawQuad(transform, color, texcoords, 0);
    }

    inline void 
    drawQuad(vec3f pos, vec2f size, vec4f color, vec2f* texcoords, uint32 textureindex) {

        BatchRenderer2DData& batchrendererdata = *batchrendererdata_;

        vec2f halfsize = size / 2.0f;

        BatchVertex vertex;
        vertex.position = { pos.x - halfsize.x, pos.y - halfsize.y };
        vertex.color = color;
        vertex.texcoord = texcoords[0];
        vertex.index = (real32)textureindex;

        batchrendererdata.localbuffer[batchrendererdata.vertexoffset + 0] = vertex;

        vertex.position = { pos.x + halfsize.x, pos.y - halfsize.y };
        vertex.color = color;
        vertex.texcoord = texcoords[1];
        vertex.index = (real32)textureindex;

        batchrendererdata.localbuffer[batchrendererdata.vertexoffset + 1] = vertex;

        vertex.position = { pos.x + halfsize.x, pos.y + halfsize.y };
        vertex.color = color;
        vertex.texcoord = texcoords[2];
        vertex.index = (real32)textureindex;

        batchrendererdata.localbuffer[batchrendererdata.vertexoffset + 2] = vertex;

        vertex.position = { pos.x - halfsize.x, pos.y + halfsize.y };
        vertex.color = color;
        vertex.texcoord = texcoords[3];
        vertex.index = (real32)textureindex;

        batchrendererdata.localbuffer[batchrendererdata.vertexoffset + 3] = vertex;

        batchrendererdata.vertexoffset += 4;

        if(batchrendererdata.vertexoffset >= batchrendererdata.vertexbuffersize / sizeof(BatchVertex)) {
            flushIntermediate();
        }
    }

    void drawTexturedQuad(vec2f pos, vec2f size, vec4f colortint, vec2f* texcoords) {

        BatchRenderer2DData& batchrendererdata = *batchrendererdata_;
        drawQuad({pos.x, pos.y, 0.0f}, size, colortint, texcoords, batchrendererdata.storedtextures - 1);
    }

    void drawTexturedQuad(vec2f pos, vec2f size, vec4f colortint) {
        vec2f texcoords[4] = {{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}};
        drawTexturedQuad(pos, size, colortint, texcoords);
    }

    void drawTexturedQuad(vec2f pos, vec2f size) {
        drawTexturedQuad(pos, size, {1.0f, 1.0f, 1.0f, 1.0f});
    }

    void drawQuad(vec2f pos, vec2f size, vec4f color) {
        vec2f texcoords[4] = {{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}};
        drawQuad({pos.x, pos.y, 0.0f}, size, color, texcoords, 0);
    }

    void drawQuadTL(vec2f pos, vec2f size, vec4f color) {
        vec2f texcoords[4] = {{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}};
        drawQuad({pos.x + size.x / 2.0f, pos.y - size.y / 2.0f, 0.0f}, size, color, texcoords, 0);
    }

    void flush() {
        BatchRenderer2DData& batchrendererdata = *batchrendererdata_;

        batchrendererdata_->stats.drawcalls += 1;
        batchrendererdata_->stats.quadcount += batchrendererdata_->vertexoffset / 4;

        batchrendererdata.rendercommands.rbo_bind(batchrendererdata.renderbuffer);
        batchrendererdata.rendercommands.bindshader(batchrendererdata.shader);

        batchrendererdata.rendercommands.bindubo(batchrendererdata.scenebuffer, 0, 0, 0);
        batchrendererdata.rendercommands.ubobuffersubdata((byte*)&batchrendererdata.scene, sizeof(SceneData), 0);

        for(int32 i = 0; i < batchrendererdata.storedtextures; i++) {
            batchrendererdata.rendercommands.texture_bind(batchrendererdata.textures[i], i);
        }
        batchrendererdata.rendercommands.drawindexed(IME_TRIANGLES, (batchrendererdata.vertexoffset / 4) * 6);
    }

    void flushIntermediate() {
        BatchRenderer2DData& batchrendererdata = *batchrendererdata_;

        mat4 proj = batchrendererdata.scene.viewprojection;
        uint32 texturecount = batchrendererdata.storedtextures;
        endScene();
        flush();
        beginScene(proj);
        batchrendererdata_->textures[1] = batchrendererdata_->textures[texturecount - 1];
        batchrendererdata.storedtextures = 2;
    }

    void flushAndReset() {
        BatchRenderer2DData& batchrendererdata = *batchrendererdata_;
        mat4 proj = batchrendererdata.scene.viewprojection;
        endScene();
        flush();
        beginScene(proj);
    }
}