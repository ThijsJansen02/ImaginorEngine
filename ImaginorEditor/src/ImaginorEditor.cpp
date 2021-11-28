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

    internal void
    drawLocalSpace(mat4 transform, RenderQueue2D* rq, gl_id shader) {

        vec3f x = {transform.data[0 + 0 * 4], transform.data[0 + 1 * 4], transform.data[0 + 2 * 4]};
        vec3f y = {transform.data[1 + 0 * 4], transform.data[1 + 1 * 4], transform.data[1 + 2 * 4]};
        vec3f z = {transform.data[2 + 0 * 4], transform.data[2 + 1 * 4], transform.data[2 + 2 * 4]};

        vec3f origin = {transform.data[3 + 0 * 4], transform.data[3 + 1 * 4], transform.data[3 + 2 * 4]};

        mat4 x_transform = calcLineSpaceMat4(origin, origin + x, 0.07f);
        mat4 y_transform = calcLineSpaceMat4(origin, origin + y, 0.07f);
        mat4 z_transform = calcLineSpaceMat4(origin, origin + z, 0.07f);

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

    void pushDebugMessage(const char* str, uint32 severity, PlatformInterface* interface) {
        Event e;
        e.destinations = IME_CONSOLE;
        e.param1 = severity;
        e.param2 = (uint64)str;
        e.source = IME_APP;
        e.type = IME_DEBUG_MESSAGE;
        interface->events.push(e);
    }

    typedef Data::Registry<2048, 40, Memory::alloc, Memory::dealloc> SceneRegistry;

    struct CacheableState {
        MemoryArena mainmemory;
        MemoryPool mainmemorypool;

        RenderQueue2D sceneRQ;
        RenderQueue2D uiRQ;

        gl_id texture;

        Data::Entity entity1;

        gl_id fontshader;
        gl_id quadshader;

        bool32 moving;
        bool32 scaling;
        bool32 rotating;

        Renderer2D::BatchRenderer2DData batchrenderdata;
    };

    struct EditorState {
        
        UI::Context uicontext;
        SceneRegistry sceneregistry;
        Data::Entity selectedentity = {0xFFFFFFFF};
        TextureAtlas font;
        CacheableState cachestate;

        UI::ElementPtr componentview;
        UI::ElementPtr sceneview;
    };

    internal void
    displayComponentsInComponentView(UI::ElementPtr windowptr, UI::Context* context, SceneRegistry* sceneregistry, UI::StyleProperties style, Data::Entity selected) {

        UI::Window& window = context->uiwindows[windowptr.dataptr].data;
        UI::Div& rootelement = context->divs[window.rootelement.dataptr].data;

        style.background = {0.3f, 0.3f, 0.3f, 1.0f};
        style.textcolor = {0.8f, 0.8f, 0.8f, 1.0f};
        style.margin = {0.0f, 0.0f, 0.0f, 0.0f};
        style.margin.bottom = 5.0f;
        style.width = 100.0f;

        for(UI::ElementPtr el : rootelement.children) {
            UI::removeElement(el, context);
        }

        rootelement.children.clear();

        if(sceneregistry->hasComponent<TagComponent>(selected)) {
            TagComponent tag = sceneregistry->getComponent<TagComponent>(selected);
            UI::addParagraph(context, window.rootelement, tag.tag, style, "tag_component");
        }

        if(sceneregistry->hasComponent<SpriteRendererComponent>(selected)) {
            SpriteRendererComponent& sc = sceneregistry->getComponent<SpriteRendererComponent>(selected);

            UI::StyleProperties style_p = style;
            style_p.background = {0.8f, 0.0f, 0.0f, 1.0f};
            UI::addParagraph(context, window.rootelement, "Sprite Component", style_p);

            UI::StyleProperties style_fs = style;
            UI::addFloatSlider(context, window.rootelement, style_fs, 4, &sc.color.x, "color");
        }

        UI::calculateUiComponentsForWindow(context, window);

    }

    bool32 onEntityClick(char* id, UI::ElementPtr element, UI::Context* context, void* userptr, Event e) {

        EditorState* stateptr = (EditorState*)userptr;

        uint32 entityid = (uint32)atoi(id);
        stateptr->selectedentity = {entityid};

        UI::Paragraph& p = context->paragraphs[element.dataptr].data;
        p.props.background = {0.6f, 0.0f, 0.0f, 1.0f};

        UI::StyleProperties style;
        style.background = {0.7, 0.7f, 0.7f, 1.0f};
        style.padding = {5.0f, 5.0f, 5.0f, 5.0f};
        style.font = &stateptr->font;
        style.glyphsize = {10.0f, 12.0f};
        style.linespacing = {1.0f};
        style.shader = stateptr->cachestate.quadshader;
        style.textcolor = {0.0f, 0.0f, 0.0f, 1.0f};
        style.margin = {5.0f, 5.0f, 5.0f, 5.0f};

        displayComponentsInComponentView(stateptr->componentview, context, &stateptr->sceneregistry, style, stateptr->selectedentity);

        return false;
    }
    bool32 onHoverEntity(char* id, UI::ElementPtr element, UI::Context* context, void* userptr, Event e) {

        UI::Paragraph& p = context->paragraphs[element.dataptr].data;
        p.props.background = {0.2f, 0.2f, 0.2f, 1.0f};
        return false;

    }

    bool32 ofHoverEntity(char* id, UI::ElementPtr element, UI::Context* context, void* userptr, Event e) {

        UI::Paragraph& p = context->paragraphs[element.dataptr].data;
        p.props.background = {0.3f, 0.3f, 0.3f, 1.0f};
        return false;
    }

    internal UI::ElementPtr
    setupSceneView(UI::Context* context, SceneRegistry* sceneregistry, UI::StyleProperties style) {
        
        UI::StyleProperties style_div = style;
        style_div.margin = {0.0f, 0.0f, 0.0f, 0.0f};
        UI::Window window;
        window.bounds = {vec2f{0.0f, 0.0f}, vec2f{300.0f, -200.0f}};
        window.context = context;
        window.rootelement = UI::addDiv(context, {UI::UI_WINDOW, 0}, style_div);
        uint32 index = (uint32)context->uiwindows.add(window);

        style.background = {0.3f, 0.3f, 0.3f, 1.0f};
        style.textcolor = {0.8f, 0.8f, 0.8f, 1.0f};
        style.margin = {0.0f, 0.0f, 0.0f, 0.0f};
        style.margin.bottom = 5.0f;
        style.width = 100.0f;

        for(auto [tag, entity] : sceneregistry->view<TagComponent>()) {

            char buffer[32];
            sprintf_s(buffer, 32, "%d", entity.index);

            UI::ElementPtr el = UI::addParagraph(context, window.rootelement, tag.tag, style, buffer);
            UI::addOnHoverToElement(el, context, onHoverEntity);
            UI::addOfHoverToElement(el, context, ofHoverEntity);
            UI::addOnClickToElement(el, context, onEntityClick);
        }

        return {UI::UI_WINDOW, index};
    }


    internal UI::ElementPtr 
    setupComponentView(UI::Context* context, SceneRegistry* sceneregistry, UI::StyleProperties style) {
        
        UI::StyleProperties style_div = style;
        style_div.margin = {0.0f, 0.0f, 0.0f, 0.0f};
        
        UI::Window window;
        window.bounds = {vec2f{0.0f, -200.0f}, vec2f{300.0f, -800.0f}};
        window.context = context;
        window.rootelement = UI::addDiv(context, {UI::UI_WINDOW, 0}, style_div);
        uint32 index = (uint32)context->uiwindows.add(window);

        return {UI::UI_WINDOW, index};
    }

    internal void
    pushSpriteRenderComponentsToRQ(SceneRegistry* scene, RenderQueue2D* rq) {
        scene->forEachPair<SpriteRendererComponent, TransformComponent>([](SpriteRendererComponent* sr, TransformComponent* tr, void* userptr){

            RenderQueue2D* rq = (RenderQueue2D*)userptr;
            RendererCommand2D command;
            command.color = sr->color;
            command.shader = sr->shader;
            command.texture = sr->texture;
            command.texcoords[0] = {0.0f, 0.0f};
            command.texcoords[0] = {1.0f, 0.0f};
            command.texcoords[0] = {1.0f, 1.0f};
            command.texcoords[0] = {0.0f, 1.0f};
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

        Data::Entity entity2 = stateptr->sceneregistry.createEntity();

        tag.tag = copyString("Entity(2)");
        tc.transform = transformMat4<real32>({0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f});
        sc.shader = state.quadshader;
        sc.texture = 0;
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

        real32 aspectratio = (real32)platform.window.width / (real32)platform.window.height;
        mat4 projection = OrthographicMat4(-10.0f, 10.0f, -10 / aspectratio, 10 / aspectratio, -100.0f, 100.0f);
        
        while(platform.events.pop(&e)) {

            UI::uiOnEvent(&stateptr->uicontext, e, platform);

        }

        platform.gfx.setviewport(0, 0, platform.window.width, platform.window.height);
        platform.gfx.clear(IME_COLOR_BUFFER | IME_DEPTH_BUFFER);
        platform.gfx.enable(IME_DEPTH_TEST);


        Renderer2D::setBatchRendererData(&state.batchrenderdata);
        Memory::setGlobal(&state.mainmemorypool);

        //start renering scene
        pushSpriteRenderComponentsToRQ(&stateptr->sceneregistry, &state.sceneRQ);
        Renderer2D::beginScene(projection);
        Renderer2D::setShader(state.quadshader);

        flushRenderQueue2D(&state.sceneRQ, platform);

        Renderer2D::endScene();
        Renderer2D::flush();

        platform.gfx.disable(IME_DEPTH_TEST);

        if(stateptr->selectedentity.index != 0xFFFFFFFF) {
            TransformComponent transform = stateptr->sceneregistry.getComponent<TransformComponent>(stateptr->selectedentity);
            drawLocalSpace(transform.transform, &state.sceneRQ, state.quadshader);
        }

        Renderer2D::beginScene(projection);
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
