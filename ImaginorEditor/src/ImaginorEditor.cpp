#include <core.h>
#include <datastructures/ecs/ecs.h>
#include <ImaginorMath/src/LinearMath/linearmath.h>
#include <ImaginorPlatform/src/applicationmain.h>
#include <datastructures/lists/arrays/basetypes/DynamicArray.h>
#include <ImaginorEngine/src/memory.h>
#include <ImaginorEngine/src/Imaginor.h>
#include <intrinsics.h>

namespace IME
{

    internal void 
    setTextureBindings(const char* formatstring, RenderCommands rendercommands) {
        char buffer[256];
        for(uint32 i = 0; i < 32; i++) {
            sprintf_s(buffer, 256, formatstring, i);
            rendercommands.shader_settexturebinding(buffer, i);
        }
    }

    internal IME::mat4 
    calcLineSpaceMat4(const vec3f& p1, const vec3f& p2, real32 thickness) {

        vec3f up = {0.0f, 1.0f, 0.0f};
        vec3f line = p2 - p1;

        vec3f middle = p1 + 0.5f * line;
        vec3f perpz = thickness * normalize(crossProduct(up, line));
        vec3f perpy = thickness * normalize(crossProduct(perpz, line));

        IME::mat4 transform = columnComposeMat4(
            {line.x, line.y, line.z, 0.0f}, 
            {perpy.x, perpy.y, perpy.y, 0.0f}, 
            {perpz.x, perpz.y, perpz.z, 0.0f}, 
            {middle.x, middle.y, middle.z, 1.0f});
        return transform;
    }

    void pushDebugMessage(const char* str, uint32 severity, PlatformInterface* interface) {
        Event e;
        e.destinations = IME_CONSOLE;
        e.param1 = severity;
        e.param2 = (uint64)str;
        e.source = IME_APP;
        e.type = IME_DEBUG_MESSAGE;
        interface->events.push(e);
    }

    struct CacheableState {
        MemoryArena mainmemory;
        MemoryPool mainmemorypool;

        RenderQueue2D renderqueue;

        gl_id texture;

        gl_id fontshader;
        gl_id quadshader;

        Renderer2D::BatchRenderer2DData batchrenderdata;
    };

    struct EditorState {
        
        Data::Registry<2048, 40, Memory::alloc, Memory::dealloc> sceneregistry;
        TextureAtlas font;
        CacheableState cachestate;
    };

    extern "C" IME_APPLICATION_INIT(applicationInit) { //bool32 applicationInit(ApplicationMemory memory, RenderCommands rendercommands)

        EditorState* stateptr = (EditorState*)platform.appmemory.persistentstorage;
        CacheableState state = stateptr->cachestate;

        state.mainmemory.base = (byte*)platform.appmemory.transientstorage;
        state.mainmemory.size = platform.appmemory.transientstoragesize;
        state.mainmemory.used = 0;

        state.mainmemorypool = createMemoryPool(&state.mainmemory, state.mainmemory.size, 2048);
        Memory::setGlobal(&state.mainmemorypool);
        Renderer2D::setBatchRendererData(&state.batchrenderdata);
        Renderer2D::setup(&state.batchrenderdata, 10000, platform.gfx, &state.mainmemorypool);

        state.fontshader = Renderer2D::loadBatchShader("batchvertexshader.glsl", "textfragment.glsl", &platform);
        state.quadshader = Renderer2D::loadBatchShader("batchvertexshader.glsl", "batchfragmentshader.glsl", &platform);

        state.renderqueue = createRenderQueue2D();

        stateptr->font = loadTextureAtlas("bitmap_font.png", 18, 6, platform, state.fontshader);

        stateptr->cachestate = state;
        return true;
    }

    extern "C" IME_APPLICATION_UPDATE(applicationUpdate) { //bool32 applicationUpdate(PlatformInterface platform)

        EditorState* stateptr = (EditorState*)platform.appmemory.persistentstorage;
        CacheableState state = stateptr->cachestate;

        Renderer2D::setBatchRendererData(&state.batchrenderdata);
        Memory::setGlobal(&state.mainmemorypool);

        RendererCommand2D command;
        command.color = {1.0f, 0.0f, 1.0f, 1.0f};

        Renderer2D::beginScene(identity());

        Renderer2D::setShader(state.quadshader);
        Renderer2D::drawQuad({0.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 0.0f, 1.0f, 1.0f});

        Renderer2D::endScene();
        Renderer2D::flush();

        stateptr->cachestate = state;
        return true;
    }
}
