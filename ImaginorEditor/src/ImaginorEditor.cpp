#include <core.h>
#include <datastructures/ecs/ecs.h>
#include <ImaginorMath/src/LinearMath/linearmath.h>
#include <ImaginorPlatform/src/applicationmain.h>
#include <datastructures/lists/arrays/basetypes/DynamicArray.h>
#include <ImaginorEngine/src/memory.h>
#include <intrinsics.h>
#include <ImaginorEngine/src/functionality.h>
#include <ImaginorMath/src/LinearMath/conversion.h>
#include <ImaginorEngine/src/assets.h>
#include "editorstate.h"
#include <ImaginorEngine/src/userinterface.h>
#include <stb/stb_treutype.h>
#include <ImaginorEngine/src/rendering/Renderer2D.h>

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
            {perpy.x, perpy.y, perpy.z, 0.0f}, 
            {perpz.x, perpz.y, perpz.z, 0.0f}, 
            {middle.x, middle.y, middle.z, 1.0f});
        return transform;
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

    ON_MOUSE_CLICK_EVENT(paragraphclick) { //ElementPtr element, Event e, Layer* uilayer, const PlatformInterface& platform)
        //UI::Paragraph* p = &uilayer->paragraphs[element.offset].data;
        UI::addConstraint(element, UI::BACKGROUND_COLOR, "1.0, 1.0, 1.0, 1.0", uilayer);
        //UI::addConstraint(element, UI::PADDING, "1.0px, 1.0px, 1.0px, 1.0px", uilayer);
        return {false, true};
    }

    ON_RESIZE_EVENT(framebufferupdate) { //ElementPtr element, Event e, Layer* uilayer, const PlatformInterface& platform)
        EditorState* stateptr = (EditorState*)uilayer->userpointer;

        platform.gfx.fbo_bind(stateptr->sceneframebuffer);
        platform.gfx.fbo_resize((uint32)newsize.width, (uint32)newsize.height);
        stateptr->colorbuffer.props.width = (uint32)newsize.width;
        stateptr->colorbuffer.props.height = (uint32)newsize.height;

        return {false, false};
    }

    extern "C" IME_APPLICATION_INIT(applicationInit) { //bool32 applicationInit(ApplicationMemory memory, RenderCommands rendercommands)

        EditorState* stateptr = (EditorState*)platform.appmemory.persistentstorage;
        stateptr->testimage = Assets::loadColorTexture8("hat32x32.png", platform, 4);
        stateptr->renderset = Rendering::initRenderSet();

        Renderer2D::setup(&stateptr->batchdata, 10000, platform);
        Renderer2D::setBatchRendererData(&stateptr->batchdata);

        stateptr->fontshader = Renderer2D::loadBatchShader("batchvertexshader.glsl", "textfragment.glsl", platform);
    	stateptr->quadshader = Renderer2D::loadBatchShader("batchvertexshader.glsl", "batchfragmentshader.glsl", platform);

        stateptr->textfont = Assets::loadFont("consolas/CONSOLA.ttf", platform, 40, 512, 512, stateptr->fontshader);
        
        stateptr->uilayer = UI::createLayer(stateptr->quadshader, platform, &stateptr->textfont);
        stateptr->uilayer.userpointer = stateptr;
        
        UI::ElementPtr renderwindow = UI::addWindowToLayer({100.0f, 1000.0f, 0.0f, 1200.0f, 900.0f}, stateptr->quadshader, "main window", platform, &stateptr->uilayer);
        UI::addOnResizeEventHandler(renderwindow, framebufferupdate, &stateptr->uilayer);
        UI::Region windowcontentregion = UI::getContentRegion(renderwindow, stateptr->uilayer);

        stateptr->sceneframebuffer = platform.gfx.fbo_create((uint32)windowcontentregion.width, (uint32)windowcontentregion.height);

        stateptr->colorbuffer.props.format = IME_RGBA;
        stateptr->colorbuffer.props.generatemipmaps = false;
        stateptr->colorbuffer.props.magfilter = IME_NEAREST;
        stateptr->colorbuffer.props.minfilter = IME_NEAREST;
        stateptr->colorbuffer.props.S = IME_REPEAT;
        stateptr->colorbuffer.props.T = IME_REPEAT;
        stateptr->colorbuffer.props.width = windowcontentregion.width;
        stateptr->colorbuffer.props.height = windowcontentregion.height;
        stateptr->colorbuffer.id = platform.gfx.fbo_createtextureattachment(IME_COLOR_ATTACHMENT0, stateptr->colorbuffer.props);

        UI::addImage(&stateptr->colorbuffer, renderwindow, &stateptr->uilayer);

        Scene::initScene(&stateptr->scene);

        Scene::SceneData* scene = &stateptr->scene;

        Scene::SpriteRendererComponent comp;
        comp.color = {1.0f, 1.0f, 1.0f, 1.0f};
        comp.shader = stateptr->quadshader;
        comp.texture = &stateptr->testimage;
        comp.textureid = comp.texture->id;

        Scene::Entity ent = Scene::addNewEntity("first entity", &stateptr->scene);
        Scene::addComponentToEntity(ent, comp, &stateptr->scene);

        Scene::Entity camera = Scene::addNewEntity("camera", &stateptr->scene);
        Scene::CameraComponent cameracomponent;
        cameracomponent.projection = perspectiveMat4(100.0f, 0.1f, 100.0f, (real32)stateptr->colorbuffer.props.width / (real32)stateptr->colorbuffer.props.height);
        Scene::addComponentToEntity(camera, cameracomponent, &stateptr->scene);
        Scene::getComponent<Scene::TransformComponent>(camera, &stateptr->scene)->transform = translationMat4(vec3f{0.0f, 0.0f, 10});
        stateptr->cameraentity = camera;
        
        //calculate the element of the window
        UI::calculateElementsForWindow(renderwindow, stateptr->uilayer);
        return true;
    }

    template<typename T>
    using ArrayList =  Data::ArrayList_<T, IME::Memory::alloc, IME::Memory::dealloc>;

    real32 dist = 0.0f;
    extern "C" IME_APPLICATION_UPDATE(applicationUpdate) { //bool32 applicationUpdate(PlatformInterface platform)

        EditorState* stateptr = (EditorState*)platform.appmemory.persistentstorage;

        Scene::SceneData* scene = &stateptr->scene;

        Event e;
        while(platform.events.pop(&e)) {
            UI::propagateEventToLayer(e, &stateptr->uilayer, platform);
        }

        Scene::CameraComponent* camera = Scene::getComponent<Scene::CameraComponent>(stateptr->cameraentity, scene);
        Scene::TransformComponent* cameratransform = Scene::getComponent<Scene::TransformComponent>(stateptr->cameraentity, scene);

        mat4 textprojection = OrthographicMat4(0.0f, platform.window.width, 0.0f, platform.window.height, -100.0f, 70.0f);
        mat4 sceneprojection = perspectiveMat4(100.0f, 0.1f, 100.0f, (real32)stateptr->colorbuffer.props.width / (real32)stateptr->colorbuffer.props.height);
        mat4 view = inverseOfOrthagonalMat4(cameratransform->transform);
        camera->projection = sceneprojection;
        //
        stateptr->renderset.renderqueues.clear();

        if(platform.keyboard.isKeyPressed(IME_KEY_W)) {
            dist+= 0.05f;
        }

        if(platform.keyboard.isKeyPressed(IME_KEY_S)) {
            dist-= 0.05f;
        }

        Scene::pushSceneToRenderSet(&stateptr->renderset, &stateptr->scene, view, sceneprojection, stateptr->sceneframebuffer, {(uint32)stateptr->colorbuffer.props.width, (uint32)stateptr->colorbuffer.props.height});

        Rendering::ArrayList<Rendering::SimpleQuadCommand> quadrq;
        quadrq.init(0);

        Rendering::RenderQueue queue;
        queue.data1 = (byte*)&quadrq[0];
        queue.count1 = quadrq.getCount();
        queue.clearcolor = {0.5f, 0.5f, 0.5f, 1.0f};
        queue.commandtype = IME::Rendering::SIMPLE_QUAD;
        queue.depthtesting = true;
        queue.bufferstoclear = 0;
        queue.projection = sceneprojection;
        queue.view = translationMat4(vec3f{0.0f, 0.0f, dist});
        queue.viewwidth = stateptr->colorbuffer.props.width;
        queue.viewheight = stateptr->colorbuffer.props.height;
        queue.rendertarget = stateptr->sceneframebuffer;    
        queue.updatescene = true;
        queue.viewx = 0;
        queue.viewy = 0;
        queue.dealloc = true;
        stateptr->renderset.renderqueues.push_back(queue);

        //pushing the ui elements to the renderset and flushing
        UI::pushLayerToRenderSet(stateptr->uilayer, &stateptr->renderset, textprojection, platform);
        Rendering::flushRenderSet(stateptr->renderset, platform);

        return true;
    }
}

