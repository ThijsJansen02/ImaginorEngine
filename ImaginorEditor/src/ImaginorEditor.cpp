#include <core.h>
#include <datastructures/ecs/ecs.h>
#include <ImaginorMath/src/LinearMath/linearmath.h>
#include <ImaginorPlatform/src/applicationmain.h>
#include <datastructures/lists/arrays/basetypes/DynamicArray.h>
#include <ImaginorEngine/src/memory.h>
#include <ImaginorEngine/src/Imaginor.h>
#include <ImaginorEngine/src/scene/Components.h>
#include <intrinsics.h>
#include <ImaginorEngine/src/functionality.h>
#include <ImaginorMath/src/LinearMath/conversion.h>
#include <ImaginorEngine/src/assets.h>
#include "editorstate.h"
#include "userinterface.h"

namespace IME::Editor
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
    calcLineSpaceMat4(const vec3f& p1, const vec3f& p2, real32 thickness, vec3f viewdirection) {

        vec3f p1p2 = p2 - p1;

        vec3f middle = p1 + 0.5f * p1p2;
        vec3f perpy = thickness * normalize(crossProduct(viewdirection, p1p2));
        vec3f perpz = thickness * normalize(crossProduct(perpy, p1p2));

        IME::mat4 transform = columnComposeMat4(
            {p1p2.x, p1p2.y, p1p2.z, 0.0f}, 
            {perpy.x, perpy.y, perpy.y, 0.0f}, 
            {perpz.x, perpz.y, perpz.z, 0.0f}, 
            {middle.x, middle.y, middle.z, 1.0f});
        return transform;
    }

    internal void
    drawLocalSpace(mat4 transform, RenderQueue2D* rq, gl_id shader, vec3f viewdir) {

        vec3f x = normalize(vec3f{transform.data[0 + 0 * 4], transform.data[0 + 1 * 4], transform.data[0 + 2 * 4]});
        vec3f y = normalize(vec3f{transform.data[1 + 0 * 4], transform.data[1 + 1 * 4], transform.data[1 + 2 * 4]});
        vec3f z = normalize(vec3f{transform.data[2 + 0 * 4], transform.data[2 + 1 * 4], transform.data[2 + 2 * 4]});

        vec3f origin = {transform.data[3 + 0 * 4], transform.data[3 + 1 * 4], transform.data[3 + 2 * 4]};

        mat4 x_transform = calcLineSpaceMat4(origin, origin + x, 0.07f, viewdir);
        mat4 y_transform = calcLineSpaceMat4(origin, origin + y, 0.07f, viewdir);
        mat4 z_transform = calcLineSpaceMat4(origin, origin + z, 0.07f, viewdir);

        RendererCommand2D command;
        command.shader = shader;
        command.texture = 0;

        command.transform = x_transform;
        command.color = {1.0f, 0.0f, 0.0f, 1.0f};
        rq->commands.push_back(command);

        command.transform = y_transform;
        command.color = {0.0f, 1.0f, 0.0f, 1.0f};
        rq->commands.push_back(command);

        command.transform = z_transform;
        command.color = {0.0f, 0.0f, 1.0f, 1.0f};
        rq->commands.push_back(command);
    }

    inline void 
    pushDebugMessage(const char* str, uint32 severity, PlatformInterface* interface) {

        Event e;
        e.destinations = IME_CONSOLE;
        e.param1 = severity;
        e.param2 = (uint64)str;
        e.source = IME_APP;
        e.type = IME_DEBUG_MESSAGE;
        interface->events.push(e);
    }

    internal void
    pushSpriteRenderComponentsToRQ(SceneRegistry* scene, RenderQueue2D* rq) {
        scene->forEachPair<SpriteRendererComponent, TransformComponent>([](SpriteRendererComponent* sr, TransformComponent* tr, void* userptr){

            RenderQueue2D* rq = (RenderQueue2D*)userptr;
            RendererCommand2D command;
            command.color = sr->color;
            command.shader = sr->shader;
            command.texture = sr->textureid;
            command.texcoords[0] = {0.0f, 0.0f};
            command.texcoords[1] = {1.0f, 0.0f};
            command.texcoords[2] = {1.0f, 1.0f};
            command.texcoords[3] = {0.0f, 1.0f};
            command.transform = tr->transform;
            rq->commands.push_back(command);

        }, rq);
    }

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

        UI::StyleProperties style;
        style.background = {0.7, 0.7f, 0.7f, 1.0f};
        style.padding = {5.0f, 5.0f, 5.0f, 5.0f};
        style.font = &stateptr->font;
        style.glyphsize = {10.0f, 12.0f};
        style.linespacing = {1.0f};
        style.shader = state.quadshader;
        style.textcolor = {0.0f, 0.0f, 0.0f, 1.0f};
        style.margin = {5.0f, 5.0f, 5.0f, 5.0f};

        state.entity1 = stateptr->sceneregistry.createEntity();
        stateptr->selectedentity = {0xFFFFFFFF};
        
        TransformComponent tc;
        SpriteRendererComponent sc;
        TagComponent tag;

        tag.tag = copyString("Entity(1)");
        tc.transform = rotationMat4({0.0f, 0.0f, toRadians(45.0f)}) * transformMat4<real32>({0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f});
        sc.shader = state.quadshader;
        sc.texture = 0;
        sc.color = {0.0f, 1.0f, 1.0f, 1.0f};

        stateptr->sceneregistry.init();

        stateptr->sceneregistry.addComponent<TagComponent>(state.entity1, tag);
        stateptr->sceneregistry.addComponent<TransformComponent>(state.entity1, tc);
        stateptr->sceneregistry.addComponent<SpriteRendererComponent>(state.entity1, sc);

        real32 aspectratio = (real32)platform.window.width / (real32)platform.window.height;
        mat4 projection = OrthographicMat4(-10.0f, 10.0f, -10 / aspectratio, 10 / aspectratio, -100.0f, 100.0f);

        stateptr->editorcamera.forward = {0.0f, 0.0f, -1.0f};
        stateptr->editorcamera.position = {0.0f, 0.0f, 0.0f};
        stateptr->editorcamera.projection = projection;

        mat4 viewprojection = calculateViewProjection(stateptr->editorcamera, {0.0f, 1.0f, 0.0f});

        Data::Entity entity2 = stateptr->sceneregistry.createEntity();

        stateptr->texture = loadColorTexture8("hat32x32.png", platform, 0);

        tag.tag = copyString("Entity(2)");
        tc.transform = transformMat4<real32>({0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f});
        sc.shader = state.quadshader;
        sc.textureid = stateptr->texture.texture;
        sc.texture = &stateptr->texture;
        
        sc.color = {1.0f, 0.0f, 1.0f, 1.0f};

        stateptr->sceneregistry.addComponent<TagComponent>(entity2, tag);
        stateptr->sceneregistry.addComponent<TransformComponent>(entity2, tc);
        stateptr->sceneregistry.addComponent<SpriteRendererComponent>(entity2, sc);

        stateptr->uicontext = UI::createContext();
        stateptr->uicontext.userptr = stateptr;
        stateptr->sceneview = setupSceneView(&stateptr->uicontext, &stateptr->sceneregistry, style);
        stateptr->componentview = setupComponentView(&stateptr->uicontext, &stateptr->sceneregistry, style);
        UI::calculateUiComponents(&stateptr->uicontext);

        state.sceneRQ = createRenderQueue2D(0);
        state.uiRQ = createRenderQueue2D(0);

        stateptr->font = loadTextureAtlas("bitmap_font.png", 18, 7, platform, state.fontshader);
        stateptr->font.offset = 32;

        stateptr->cachestate = state;
        return true;
    }

    extern "C" IME_APPLICATION_UPDATE(applicationUpdate) { //bool32 applicationUpdate(PlatformInterface platform)


        EditorState* stateptr = (EditorState*)platform.appmemory.persistentstorage;
        CacheableState state = stateptr->cachestate;
        Event e;
        
        while(platform.events.pop(&e)) {
            UI::uiOnEvent(&stateptr->uicontext, e, platform);
        }

        if(platform.keyboard.isKeyPressed('W')) {
            stateptr->editorcamera.position.y += 5.0f * platform.time.lastframetime;
        }
        if(platform.keyboard.isKeyPressed('S')) {
            stateptr->editorcamera.position.y -= 5.0f * platform.time.lastframetime;
        }
        if(platform.keyboard.isKeyPressed('A')) {
            stateptr->editorcamera.position.x -= 5.0f * platform.time.lastframetime;
        }
        if(platform.keyboard.isKeyPressed('D')) {
            stateptr->editorcamera.position.x += 5.0f * platform.time.lastframetime;
        }
        if(platform.keyboard.isKeyPressed('Q')) {
            stateptr->editorcamera.yaw -= 45.0f * platform.time.lastframetime;
        }
        if(platform.keyboard.isKeyPressed('E')) {
            stateptr->editorcamera.yaw += 45.0f * platform.time.lastframetime;
        }


        real32 aspectratio = (real32)platform.window.width / (real32)platform.window.height;
        mat4 projection = OrthographicMat4(-10.0f, 10.0f, -10 / aspectratio, 10 / aspectratio, -100.0f, 100.0f);

        stateptr->editorcamera.projection = projection;
        mat4 viewprojection = calculateViewProjection(stateptr->editorcamera, {0.0f, 1.0f, 0.0f});

        platform.gfx.setviewport(0, 0, platform.window.width, platform.window.height);
        platform.gfx.clear(IME_COLOR_BUFFER | IME_DEPTH_BUFFER);
        platform.gfx.enable(IME_DEPTH_TEST);


        Renderer2D::setBatchRendererData(&state.batchrenderdata);
        Memory::setGlobal(&state.mainmemorypool);

        //start rendering scene
        pushSpriteRenderComponentsToRQ(&stateptr->sceneregistry, &state.sceneRQ);

        Renderer2D::beginScene(viewprojection);
        Renderer2D::setShader(state.quadshader);

        flushRenderQueue2D(&state.sceneRQ, platform);

        Renderer2D::endScene();
        Renderer2D::flush();

        //render gizmo
        platform.gfx.disable(IME_DEPTH_TEST);
        if(stateptr->selectedentity.index != 0xFFFFFFFF) {
            TransformComponent transform = stateptr->sceneregistry.getComponent<TransformComponent>(stateptr->selectedentity);
            drawLocalSpace(transform.transform, &state.sceneRQ, state.quadshader, {0.0f, 0.0f, -1.0f});
        }

        Renderer2D::beginScene(viewprojection);
        Renderer2D::setShader(state.quadshader);

        flushRenderQueue2D(&state.sceneRQ, platform);

        Renderer2D::endScene();
        Renderer2D::flush();

        platform.gfx.enable(IME_DEPTH_TEST);

        //start rendering ui
        mat4 uiprojection = OrthographicMat4(0.0f, (real32)platform.window.width, -(real32)platform.window.height, 0.0f, -100.0f, 100.0f);
        UI::updateUi(&stateptr->uicontext, platform);
        UI::pushElementsToRQ(stateptr->uicontext, &state.uiRQ, state.quadshader);

        Renderer2D::beginScene(uiprojection);
        Renderer2D::setShader(state.quadshader);
        //Renderer2D::drawQuad({0.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 0.0f, 1.0f, 1.0f});

        flushRenderQueue2D(&state.uiRQ, platform);

        Renderer2D::endScene();
        Renderer2D::flush();


        stateptr->cachestate = state;
        return true;
    }
}
