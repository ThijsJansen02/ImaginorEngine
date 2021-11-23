#include <core.h>
#include <datastructures/ecs/ecs.h>
#include <LinearMath/vec2.h>
#include "LinearMath/vec3.h"
#include <ImaginorPlatform/src/applicationmain.h>
#include <datastructures/lists/arrays/basetypes/DynamicArray.h>
#include <ImaginorEngine/src/memory.h>
#include <LinearMath/mat3.h>
#include <LinearMath/mat4.h>
#include <stb_image/stb_image.h>
#include <ImaginorEngine/src/rendering/Renderer2D.h>
#include "textrenderering.h"
#include "rendering/RenderQueue.h"
#include "userinterface/Window.h"
#include "userinterface/Context.h"
#include <typeinfo>
#include "scene/Components.h"

namespace IME
{
    struct uboMatrices {
        mat4 model;
        mat4 view;
        mat4 projection;
    };
    
    struct ReferencableApplicationState {
        vec3f value;
        Data::Registry<100, 20, Memory::alloc, Memory::dealloc> sceneregistry;
    };

    struct NonReferenceAbleApplicationState {

        UI::Context uicontext;
        TextureAtlas textureatlas;  

        real32 time = 0;
        Renderer2D::BatchRenderer2DData batchrendererdata;

        gl_id textshader;
        gl_id quadshader;

        gl_id texture;
        RenderQueue2D renderqueue2D;

        MemoryPool mainmemory;
        MemoryArena memoryarena;
        vec2f camerapos;
    };

    struct ApplicationState {
        ReferencableApplicationState refstate;
        NonReferenceAbleApplicationState nonrefstate;
    }

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

    struct Transform {
        mat4 transform;
    };

    template<typename T>
    struct Component {
        T data;
    };

    extern "C" IME_APPLICATION_INIT(applicationInit) { //bool32 applicationInit(ApplicationMemory memory, RenderCommands rendercommands)

        IME::NonReferenceAbleApplicationState state = {  };
        IME::ApplicationState* stateptr = (IME::ApplicationState*)platform.appmemory.persistentstorage;
        IME::ReferencableApplicationState* refstate = &stateptr->refstate;
        IME::NonReferenceAbleApplicationState* nonrefstate = &stateptr->nonrefstate;
        *refstate = {};

        Renderer2D::setBatchRendererData(&state.batchrendererdata);
        Memory::setGlobal(&state.mainmemory);

        state.memoryarena.size = platform.appmemory.transientstoragesize;
        state.memoryarena.base = (byte*)platform.appmemory.transientstorage;
        state.memoryarena.used = 0;
        state.mainmemory = createMemoryPool(&state.memoryarena, state.memoryarena.size, 1024);

        state.renderqueue2D = IME::createRenderQueue2D();

        state.textshader = Renderer2D::loadBatchShader("batchvertexshader.glsl", "textfragment.glsl", &platform);
        state.quadshader = Renderer2D::loadBatchShader("batchvertexshader.glsl", "batchfragmentshader.glsl", &platform);

        state.textureatlas = loadTextureAtlas("bitmap_font.png", 18, 7, platform, state.textshader);
        state.textureatlas.offset = 32;

        state.camerapos = {0.0f, 0.0f};

        stateptr->refstate.value = {30000.0f, 1.0f, 10.0f};

        state.uicontext = UI::createContext();

        Event event;
        event.type = IME_DEBUG_MESSAGE;
        event.source = IME_APP;
        event.destinations = IME_CONSOLE;
        event.param1 = IME_WARN;

        UI::StyleProperties style;
        style.background = {0.3, 0.3f, 0.3f, 1.0f};
        style.padding = {5.0f, 5.0f, 5.0f, 5.0f};
        style.font = &nonrefstate->textureatlas;
        style.glyphsize = {10.0f, 15.0f};
        style.linespacing = {1.0f};
        style.shader = state.quadshader;
        style.textcolor = {0.6f, 0.1f, 0.1f, 1.0f};
        style.margin = {5.0f, 5.0f, 5.0f, 5.0f};

        UI::Window uiwindow;
        uiwindow.context = &stateptr->nonrefstate.uicontext;
        uiwindow.bounds.topleft = {200.0f, -200.0f};
        uiwindow.bounds.bottomright = {600.0f, -600.0f};
        uiwindow.rootelement = UI::addDiv(&state.uicontext, {UI::UI_WINDOW, 0}, style);
        state.uicontext.uiwindows.push_back(uiwindow);

        style.margin = {0.0f, 0.0f, 0.0f, 0.0f};
        style.background = {0.5f, 0.5f, 0.5f, 1.0f};

        UI::addParagraph(&state.uicontext, 
            uiwindow.rootelement, 
            "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.",
            style);


        style.margin.top = 30.0f;
        UI::addFloatSlider(&state.uicontext, uiwindow.rootelement, style, 3, &refstate->value.x, "float slider");
        UI::calculateUiComponents(&state.uicontext);

        Renderer2D::setup(&state.batchrendererdata, 10000, platform.gfx, &state.mainmemory);

        TextureProperties props = {};
        props.format = IME_RGBA;
        props.generatemipmaps = false;
        props.magfilter = IME_NEAREST;
        props.minfilter = IME_NEAREST;
        props.S = IME_REPEAT;
        props.T = IME_REPEAT;
        
        FileBuffer texturedata = platform.io.debug_readfile("hat32x32.png", nullptr);
        int32 ch;
        char* decompressed = (char*)stbi_load_from_memory((const stbi_uc*)texturedata.data, texturedata.size, &props.width, &props.height, &ch, 4);
        state.texture = platform.gfx.texture_create(props, (byte*)decompressed, IME_RGBA, IME_UINT8);
        stbi_image_free(decompressed);
        platform.io.debug_releasefilememory(&texturedata);

        refstate->sceneregistry.init();
        Data::Entity entity = refstate->sceneregistry.createEntity();
        Data::Entity entity2 = refstate->sceneregistry.createEntity();
        Data::Entity entity3 = refstate->sceneregistry.createEntity();

        TransformComponent transform;
        SpriteRendererComponent sprite;

        sprite.texture = state.texture;
        sprite.color  = {1.0f, 1.0f, 1.0f, 1.0f};
        sprite.shader = state.quadshader;

        transform.transform = transformMat4(vec3f{5.0f, 5.0f, 0.0f}, vec3f{3.0f, 3.0f, 1.0f});
        refstate->sceneregistry.addComponent<TransformComponent>(entity, transform);
        refstate->sceneregistry.addComponent<SpriteRendererComponent>(entity, sprite);

        transform.transform = transformMat4(vec3f{-5.0f, 5.0f, 0.0f}, vec3f{3.0f, 3.0f, 1.0f});
        refstate->sceneregistry.addComponent<TransformComponent>(entity2, transform);
        refstate->sceneregistry.addComponent<SpriteRendererComponent>(entity2, sprite);

        transform.transform = transformMat4(vec3f{-5.0f, -5.0f, 0.0f}, vec3f{3.0f, 3.0f, 1.0f});
        refstate->sceneregistry.addComponent<TransformComponent>(entity3, transform);
        refstate->sceneregistry.addComponent<SpriteRendererComponent>(entity3, sprite);

        event.type = IME_DEBUG_MESSAGE;
        event.source = IME_APP;
        event.destinations = IME_CONSOLE;
        event.param2 = (uint64)"inited application!";
        event.param1 = IME_WARN;
        platform.events.push(event);

        ((ApplicationState*)platform.appmemory.persistentstorage)->nonrefstate = state;
        return true;
    }

    extern "C" IME_APPLICATION_UPDATE(applicationUpdate) { //bool32 applicationUpdate(PlatformInterface platform)

        ApplicationState* stateptr = (ApplicationState*)platform.appmemory.persistentstorage;
        NonReferenceAbleApplicationState state = ((ApplicationState*)platform.appmemory.persistentstorage)->nonrefstate;
        IME::ReferencableApplicationState* refstate = &stateptr->refstate;
        Renderer2D::setBatchRendererData(&state.batchrendererdata);
        Memory::setGlobal(&state.mainmemory);

        bool pressedw = false;

        platform.gfx.enable(IME_DEPTH_TEST);

        Event event;
        while(platform.events.pop(&event)) {
            UI::uiOnEvent(&state.uicontext, event, platform);
            if(event.type == IME_MOUSE_BUTTON_PRESSED) {

                char buffer[256];

                sprintf_s(buffer, 256, "button pressed {%f, %f}", platform.controllers->sleft_thumb.x, platform.controllers->sleft_thumb.y);
                pushDebugMessage(buffer, IME_INFO, &platform);
            }
        }

        UI::updateUi(&state.uicontext, platform);

        real32 speed = 10.0f;
        if(platform.keyboard.isKeyPressed('W')) {
            state.camerapos.y += speed * platform.time.lastframetime;
        }
        if(platform.keyboard.isKeyPressed('S')) {
            state.camerapos.y -= speed * platform.time.lastframetime;
        }
        if(platform.keyboard.isKeyPressed('A')) {
            state.camerapos.x -= speed * platform.time.lastframetime;
        }
        if(platform.keyboard.isKeyPressed('D')) {
            state.camerapos.x += speed * platform.time.lastframetime;
        }

        mat4 view = translationMat4(vec3f{-state.camerapos.x, -state.camerapos.y, 0.0f});

        platform.gfx.clear(IME_STENCIL_BUFFER | IME_COLOR_BUFFER | IME_DEPTH_BUFFER);
        state.time += 1.0f;

        platform.gfx.setviewport(0, 0, platform.window.width, platform.window.height);

        IME::vec3<real32> p1 = {-5.0f, -7.0f, -1.0f};
        IME::vec3<real32> p2 = { 5.0f,  7.0f, 0.0f};

        mat4 linespace = IME::calcLineSpaceMat4(p1, p2, 0.01);

        real32 aspectratio = (real32)platform.window.width / (real32)platform.window.height;
        real32 top = 10.0f;
        real32 bottom = -10.0f;
        real32 left = aspectratio * -10;
        real32 right =  aspectratio * 10.0f;
        mat4 projection = OrthographicMat4(left, right, bottom, top, -100.0f, 100.0f);

        vec2f texcoords[4] {
            {0.0f, 0.0f},
            {1.0f, 0.0f},
            {1.0f, 1.0f},
            {0.0f, 1.0f}
        };

        real32 width = right - left;
        real32 height = top - bottom;

#if 0
        uint32 sqrtnsprites = (uint32)squareRootReal32(stateptr->refstate.value.z);
        for(uint32 y = 0; y < sqrtnsprites; y++) {
            for(uint32 x = 0; x < sqrtnsprites; x++) {
                
                RendererCommand2D command;
                if(x * y % 2 == 0) {
                    command.shader = state.quadshader;
                    command.texture = 0;
                    command.transform = transformMat4(vec3f{(real32)x, (real32)y, 0.0f}, vec3f{1.0f, 1.0f, 1.0f});
                    command.color = {x * 1.0f / (real32)sqrtnsprites, y * 1.0f / (real32)sqrtnsprites, 0.0f, 1.0f};
                } else {
                    command.shader = state.quadshader;
                    command.texture = state.texture;
                    command.transform = transformMat4(vec3f{(real32)x, (real32)y, 0.0f}, vec3f{1.0f, 1.0f, 1.0f});
                    command.color = {1.0f, 1.0f, 1.0f, 1.0f};
                    for(uint32 i = 0; i < 4; i++) {
                        command.texcoords[i] = texcoords[i];
                    }
                }
                state.renderqueue2D.commands.push_back(command);
            }
        }
#endif

        refstate->sceneregistry.forEachPair<SpriteRendererComponent, TransformComponent>([](SpriteRendererComponent* spriterenderer, TransformComponent* transform, void* userptr){

            NonReferenceAbleApplicationState* stateptr = (NonReferenceAbleApplicationState*)userptr;

            vec2f texcoords[4] {
                {0.0f, 0.0f},
                {1.0f, 0.0f},
                {1.0f, 1.0f},
                {0.0f, 1.0f}
            };

            RendererCommand2D command;
            command.color = spriterenderer->color;
            command.shader = spriterenderer->shader;
            command.texture = spriterenderer->texture;

            for(uint32 i = 0; i < 4; i++) {
                command.texcoords[i] = texcoords[i];
            }
            command.transform = transform->transform;
            stateptr->renderqueue2D.commands.push_back(command);

        }, &state);

        Renderer2D::beginScene(projection * view);

        flushRenderQueue2D(&state.renderqueue2D, platform);

        Renderer2D::endScene();
        Renderer2D::flush();

        mat4 uispace = OrthographicMat4(0.0f, (real32)platform.window.width, -(real32)platform.window.height, 0.0f, -100.0f, 100.0f);

        real32 y = 0.0f;
        vec2f glyphsize = { 15.0f, 15.0f };

        char buffer[256];
        sprintf_s(buffer, 256, "fps: %f", 1.0f / platform.time.lastframetime);
        y += drawStringFromTextureAtlas(buffer, {0.0f, y}, glyphsize, state.textureatlas, platform.window.width, {1.0f, 1.0f, 1.0f, 1.0f}, &state.renderqueue2D);

        sprintf_s(buffer, 256, "memory used: %zu memory available: %zd", state.mainmemory.used, state.mainmemory.size - state.mainmemory.used);
        y += drawStringFromTextureAtlas(buffer, {0.0f, y}, glyphsize, state.textureatlas, platform.window.width, {1.0f, 1.0f, 1.0f, 1.0f}, &state.renderqueue2D);

        sprintf_s(buffer, 256, "largest poolchunk: %u", state.mainmemory.largestpoolchunk);
        y += drawStringFromTextureAtlas(buffer, {0.0f, y}, glyphsize, state.textureatlas, platform.window.width, {1.0f, 1.0f, 1.0f, 1.0f}, &state.renderqueue2D);

        Renderer2D::stats stats = Renderer2D::getStats();
        Renderer2D::resetStats();

        sprintf_s(buffer, 256, "drawcalls: %u", stats.drawcalls);
        y += drawStringFromTextureAtlas(buffer, {0.0f, y}, glyphsize, state.textureatlas, platform.window.width, {1.0f, 1.0f, 1.0f, 1.0f}, &state.renderqueue2D);

        sprintf_s(buffer, 256, "quads: %u", stats.quadcount);
        y += drawStringFromTextureAtlas(buffer, {0.0f, y}, glyphsize, state.textureatlas, platform.window.width, {1.0f, 1.0f, 1.0f, 1.0f}, &state.renderqueue2D);

        UI::drawUiComponents(state.uicontext, &state.renderqueue2D, state.quadshader);

        Renderer2D::beginScene(uispace);

        flushRenderQueue2D(&state.renderqueue2D, platform);

        Renderer2D::endScene();
        Renderer2D::flush();

        ((ApplicationState*)platform.appmemory.persistentstorage)->nonrefstate = state;
        return true;
    }
}
