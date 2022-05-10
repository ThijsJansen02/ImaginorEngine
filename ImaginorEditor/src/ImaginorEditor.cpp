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
#include <ImaginorEngine/src/userinterface.h>
#include <stb/stb_treutype.h>
#include <ImaginorEngine/src/rendering/Renderer2D.h>
#include "scripts/cameracontroller.h"
#include <ImaginorEngine/src/components.h>
#include <datastructures/maps/hashmap.h>
#include "editorstate.h"
#include "ui.h"

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
        UI::addConstraint(element, UI::BACKGROUND_COLOR, "1.0, 1.0, 1.0, 1.0");
        //UI::addConstraint(element, UI::PADDING, "1.0px, 1.0px, 1.0px, 1.0px", uilayer);
        return {false, true};
    }

    ON_RESIZE_EVENT(framebufferupdate) { //ElementPtr element, Event e, Layer* uilayer, const PlatformInterface& platform)
        EditorState* stateptr = (EditorState*)uilayer->userpointer;

        char buffer[64];
        sprintf_s(buffer, 64, "%.0fpx", newsize.height);

        UI::removeConstraint(element, UI::HEIGHT);
        UI::addConstraint(element, UI::HEIGHT, buffer);

        Assets::resizeFrameBuffer((uint32)newsize.x, (uint32)newsize.y, stateptr->sceneframebuffer, platform);
        return {false, true};
    }

    ON_RESIZE_EVENT(basewindowresize) { //ElementPtr element, Event e, Layer* uilayer, const PlatformInterface& platform)
        
        char buffer[64];
        sprintf_s(buffer, 64, "%.0fpx", newsize.height);

        UI::removeConstraint(element, UI::HEIGHT);
        UI::addConstraint(element, UI::HEIGHT, buffer);

        return {false, true};
    }

    struct SceneUniformBuffer  {
        mat4 viewprojection;
        mat4 transform;
    };

    float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };

    LayoutChunk chunks[] = { {IME_FLOAT3, 0, false}, {IME_FLOAT3, 1, false}, {IME_FLOAT2, 2, false}};
    global_var Assets::MeshLayout cubebufferlayout = { chunks, sizeof(chunks) / sizeof(LayoutChunk) };

    extern "C" IME_APPLICATION_INIT(applicationInit) { //bool32 applicationInit(ApplicationMemory memory, RenderCommands rendercommands)

        EditorState* stateptr = (EditorState*)platform.appmemory.persistentstorage;
        Assets::initLibrary(&stateptr->assetlibrary);

        //platform.gfx.disable(IME_BLEND);

        Assets::Library* lib = &stateptr->assetlibrary;

        stateptr->testimage = Assets::loadColorTexture8("hat32x32.png", platform, 4, "hat_D", lib);
        stateptr->selectedentity.index = 0xFFFFFFFF;

        Renderer2D::setup(&stateptr->batchdata, 10000, platform);
        Renderer2D::setBatchRendererData(&stateptr->batchdata);

        stateptr->renderset = Rendering::initRenderSet(IME_MEGABYTE(16), lib, platform);

        stateptr->fontshader = Renderer2D::loadBatchShader("batchvertexshader.glsl", "textfragment.glsl", platform);
    	stateptr->quadshader = Renderer2D::loadBatchShader("batchvertexshader.glsl", "batchfragmentshader.glsl", platform);

        Assets::Shader* shader = Assets::loadShader("vertex.glsl", "fragment.glsl", platform, "meshshader", lib);
        platform.gfx.bindshader(shader->shaderid);
        platform.gfx.setuniformbinding("Scene", 1);
        platform.gfx.setuniformbinding("Transforms", 0);

        Assets::loadColorTexture8("container.png", platform, 4, "container", lib);

        stateptr->textfont = Assets::loadFont("consolas/CONSOLA.ttf", platform, 40, 512, 512, stateptr->fontshader, "consolas_reg", lib);
        
        stateptr->examplemesh = Assets::createMesh((byte*)vertices, sizeof(vertices), nullptr, 0, cubebufferlayout, platform, "cube", lib);

        //userinterface setup
        stateptr->uilayer = UI::createLayer(stateptr->quadshader, &platform, stateptr->textfont, lib);
        stateptr->uilayer.userpointer = stateptr;
        
        UI::Layer* uilayer = &stateptr->uilayer;

        char buffer[64];
        sprintf_s(buffer, 64, "%.0fpx", (real32)platform.window.height);
        UI::addConstraint(uilayer->basewindow, UI::HEIGHT, buffer);
        UI::addConstraint(uilayer->basewindow, UI::BACKGROUND_COLOR, "1.0, 0.0, 0.0, 1.0");
        UI::addOnResizeEventHandler(uilayer->basewindow, basewindowresize, uilayer);

        UI::ElementPtr ds = UI::addDockingSpace(uilayer->basewindow, uilayer);
        UI::addConstraint(ds, UI::HEIGHT, "100%");
        UI::addConstraint(ds, UI::WIDTH, "100%");
        UI::addConstraint(ds, UI::BACKGROUND_COLOR, "0.3, 0.3, 0.3, 1.0");
        
        UI::ElementPtr renderwindow = UI::addWindowToLayer({300.0f, 1000.0f, 0.0f, 1200.0f, 900.0f}, stateptr->quadshader, "main window", platform, &stateptr->uilayer, UI::NONE);
        UI::addConstraint(renderwindow, UI::BORDER, "1.0px, 1.0px, 1.0px, 1.0px");
        UI::addConstraint(renderwindow, UI::BACKGROUND_COLOR, "0.3, 0.3, 0.3, 1.0");
        UI::addTag(renderwindow, "render_window", uilayer);
        
        /*
        UI::ElementPtr examplewindow = UI::addWindowToLayer({100.0f, 800.0f, 0.0f, 300.0f, 300.0f}, 0, "example window", platform, uilayer);
        UI::addConstraint(examplewindow, UI::BACKGROUND_COLOR, "0.3, 0.3, 0.3, 1.0");
        UI::addConstraint(examplewindow, UI::BORDER, "1.0px, 1.0px, 1.0px, 1.0px");
        UI::ElementPtr examplewindow2 = UI::addWindowToLayer({400.0f, 800.0f, 0.0f, 300.0f, 300.0f}, 0, "example window 2", platform, uilayer);
        UI::addConstraint(examplewindow2, UI::BACKGROUND_COLOR, "0.3, 0.3, 0.3, 1.0");
        UI::addConstraint(examplewindow2, UI::BORDER, "1.0px, 1.0px, 1.0px, 1.0px");
        */

        //calculate the element of the window
        UI::calculateAllElements(stateptr->uilayer);

        UI::Region windowcontentregion = UI::getContentRegion(renderwindow);

        stateptr->sceneframebuffer = Assets::createFrameBuffer((uint32)windowcontentregion.width, (uint32)windowcontentregion.height, platform, "sceneframebuffer", lib);
        stateptr->idbuffer = Assets::addTextureAttachmentToFramebuffer(stateptr->sceneframebuffer, IME_RED_INT32, IME_COLOR_ATTACHMENT1, platform, "sceneidbuffer", lib);
        stateptr->colorbuffer = Assets::addTextureAttachmentToFramebuffer(stateptr->sceneframebuffer, IME_RGB, IME_COLOR_ATTACHMENT0, platform, "scenecolorbuffer", lib);
        Assets::addRenderBufferAttachmentToFramebuffer(stateptr->sceneframebuffer, IME_DEPTH24_STENCIL8, IME_DEPTH_STENCIL_ATTACHMENT, platform, "scenedepthbuffer", lib);
        
        //add the sceneimage to the renderwindow
        UI::ElementPtr sceneimage = UI::addImage(stateptr->colorbuffer, renderwindow, &stateptr->uilayer);
        UI::addConstraint(sceneimage, UI::HEIGHT, "100%");
        UI::addConstraint(sceneimage, UI::WIDTH, "100%");
        UI::addOnClickEventHandler(sceneimage, click_view_port, uilayer);
    
        Scene::initScene(&stateptr->scene, lib, platform);
        Scene::SceneData* scene = &stateptr->scene;

        Scene::SpriteRendererComponent comp;
        comp.color = {1.0f, 1.0f, 1.0f, 1.0f};
        comp.shader = stateptr->quadshader;
        comp.texture = stateptr->testimage;
        comp.textureid = comp.texture->id;

        Scene::MeshRenderComponent meshcomp;
        meshcomp.mesh = &lib->meshes.get(Assets::String::createWrapper("cube"))->v;
        meshcomp.rbo_id = meshcomp.mesh->renderbuffer;
        meshcomp.shader_id = shader->shaderid;
        meshcomp.texture_id = lib->textures.get(Assets::String::createWrapper("container"))->v.id;

        Scene::Entity ent = Scene::addNewEntity("first entity", scene);
        Scene::addComponentToEntity(ent, meshcomp, scene);

        Scene::Entity ent2 = Scene::addNewEntity("second entity", scene);
        mat4* trans = &Scene::getComponent<Scene::TransformComponent>(ent2, scene)->transform;
        *trans = translationMat4(vec3f{0.0f, 0.0f, 5.0f});
        Scene::addComponentToEntity(ent2, meshcomp, scene);

        Scene::Entity camera = Scene::addNewEntity("camera", &stateptr->scene);
        Scene::CameraComponent cameracomponent;
        cameracomponent.projection = perspectiveMat4(100.0f, 0.1f, 100.0f, (real32)stateptr->colorbuffer->props.width / (real32)stateptr->colorbuffer->props.height);
        Scene::addComponentToEntity(camera, cameracomponent, &stateptr->scene);
        Scene::getComponent<Scene::TransformComponent>(camera, &stateptr->scene)->transform = translationMat4(vec3f{0.0f, 0.0f, 10});
        stateptr->cameraentity = camera; 

        Scene::ScriptComponent script;
        script.alloc = alloc_cameracontroller;
        script.dealloc = dealloc_cameracontroller;
        script.init = cameracontroller_on_init;
        script.onupdate = cameracontroller_on_update;
        script.onevent = cameracontroller_on_update;

        script.data = script.alloc();
        script.init(script.data, scene, camera);
        Scene::addComponentToEntity(camera, script, scene);

        UI::ElementPtr scenewindow = loadSceneView(stateptr, {0.0f, 1000.0f, 0.0f, 300.0f, 400.0f}, platform);
        UI::ElementPtr componentwindow = loadComponentView(stateptr, {0.0f, 1000.0f, 0.0f, 300.0f, 400.0f}, platform);
        
        UI::dockWindow(scenewindow, ds, false, 0, uilayer);
        UI::dockWindow(componentwindow, ds, true, 1, uilayer);
        UI::dockWindow(renderwindow, ds, false, 1, uilayer);

        UI::calculateAllElements(*uilayer);
        UI::addOnResizeEventHandler(renderwindow, framebufferupdate, &stateptr->uilayer);

        return true;
    }

    template<typename T>
    using ArrayList =  Data::ArrayList_<T, IME::Memory::alloc, IME::Memory::dealloc>;

    real32 dist = 0.0f;
    extern "C" IME_APPLICATION_UPDATE(applicationUpdate) { //bool32 applicationUpdate(PlatformInterface platform)

        EditorState* stateptr = (EditorState*)platform.appmemory.persistentstorage;
        Scene::SceneData* scene = &stateptr->scene;
        UI::Layer* uilayer = &stateptr->uilayer;

        UI::ElementPtr renderwindow = UI::getElementByTag("render_window", uilayer);

        Event e;
        while(platform.events.pop(&e)) {
            UI::propagateEventToLayer(e, &stateptr->uilayer, platform);

            if(uilayer->focussedwindow == renderwindow) {
                Scene::propagateEventToScripts(e, platform, scene);
            }
        }

        if(uilayer->focussedwindow == renderwindow) { 
            Scene::updateScripts(platform, scene);
        }

        platform.gfx.disable(IME_DEPTH_TEST);

        Scene::CameraComponent* camera = Scene::getComponent<Scene::CameraComponent>(stateptr->cameraentity, scene);
        Scene::TransformComponent* cameratransform = Scene::getComponent<Scene::TransformComponent>(stateptr->cameraentity, scene);

        mat4 textprojection = OrthographicMat4(0.0f, platform.window.width, 0.0f, platform.window.height, -100.0f, 100.0f);
        mat4 sceneprojection = perspectiveMat4(70.0f, 0.1f, 100.0f, (real32)stateptr->colorbuffer->props.width / (real32)stateptr->colorbuffer->props.height);
        mat4 view = inverseOfOrthagonalMat4(cameratransform->transform);
        camera->projection = sceneprojection;

        stateptr->renderset.renderqueues.clear();
        Scene::pushSceneToRenderSet(&stateptr->renderset, &stateptr->scene, view, sceneprojection, stateptr->sceneframebuffer, {(uint32)stateptr->colorbuffer->props.width, (uint32)stateptr->colorbuffer->props.height}, platform);

        //pushing the ui elements to the renderset and flushing
        UI::pushLayerToRenderSet(stateptr->uilayer, &stateptr->renderset, textprojection, platform);
        Rendering::flushRenderSet(stateptr->renderset, platform);

        return true;
    }
}

