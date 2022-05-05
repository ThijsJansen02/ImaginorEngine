#include "scene.h"
#include "components.h"

namespace IME::Scene {

    void initScene(SceneData* scene, Assets::Library* library, const PlatformInterface& platform) {
        scene->scenebuffer = Assets::createUnformBuffer(sizeof(mat4), nullptr, IME_DYNAMIC_DRAW, platform, "scene_databuffer", library);
        scene->registry.init();
    }

    void updateScripts(const PlatformInterface& platform, SceneData* scene) {
        for(SceneRegistry::ComponentData<Scene::ScriptComponent> component : scene->registry.view<Scene::ScriptComponent>()) {
            if(component.used) {
                component.component.onupdate(component.component.data, scene, platform);
            }
        }
    }

    void propagateEventToScripts(Event e, const PlatformInterface& platform, SceneData* scene) {
        for(SceneRegistry::ComponentData<Scene::ScriptComponent> component : scene->registry.view<Scene::ScriptComponent>()) {
            if(component.used) {
                component.component.onevent(component.component.data, e, scene, platform);
            }
        }
    }

    void meshToRenderSet(MeshRenderComponent* mr, TransformComponent* tf,  void* userpointer) {

        Rendering::MeshRQ* meshrq = (Rendering::MeshRQ*)userpointer;
        
        Rendering::MeshObjectCommand command;
        command.shader = mr->shader_id;
        command.rbo = mr->rbo_id;
        command.texture = mr->texture_id;        
        command.transform = tf->transform;
        meshrq->push_back(command);
    }

    void spriteToRenderSet(SpriteRendererComponent* sr, TransformComponent* tf, void* userpointer) {

        Rendering::ComplexQuadRQ* quadrq = (Rendering::ComplexQuadRQ*)userpointer;
        
        Rendering::ComplexQuadCommand command;
        command.color = sr->color;
        command.shader = sr->shader;
        command.texcoords[0] = {0.0f, 0.0f};
        command.texcoords[1] = {1.0f, 0.0f};
        command.texcoords[2] = {1.0f, 1.0f};
        command.texcoords[3] = {0.0f, 1.0f};

        command.texture = sr->textureid;
        
        command.transform = tf->transform;
        quadrq->push_back(command);
    }

    void pushSceneToRenderSet(Rendering::RenderSet* rs, SceneData* scene, mat4 view, mat4 projection, gl_id framebuffer, vec2<uint32> framebuffersize, PlatformInterface& platform) {
        
        Rendering::ComplexQuadRQ quadrq;
        Rendering::MeshRQ meshrq;

        mat4 viewprojection = projection * view;

        platform.gfx.bindubo(scene->scenebuffer->id, 0, 0, 0);
        platform.gfx.ubobuffersubdata((byte*)&viewprojection, sizeof(mat4), 0);

        quadrq.init(0);
        meshrq.init(0);

        scene->registry.forEachPair<SpriteRendererComponent, TransformComponent, spriteToRenderSet>((void*)&quadrq);
        scene->registry.forEachPair<MeshRenderComponent, TransformComponent, meshToRenderSet>((void*)&meshrq);

        Rendering::RenderQueue queue;

        if(quadrq.getCount() > 0) {
            queue.data1 = (byte*)&quadrq[0];
            queue.count1 = quadrq.getCount();
            queue.clearcolor = {0.5f, 0.5f, 0.5f, 1.0f};
            queue.commandtype = IME::Rendering::COMPLEX_QUAD;
            queue.depthtesting = true;
            queue.bufferstoclear = IME::IME_DEPTH_BUFFER | IME::IME_COLOR_BUFFER;
            queue.projection = projection;
            queue.view = view;
            queue.viewheight = framebuffersize.height;
            queue.viewwidth = framebuffersize.width;
            queue.rendertarget = framebuffer;
            queue.updatescene = true;
            queue.viewx = 0;
            queue.viewy = 0;
            queue.dealloc = true;
            rs->renderqueues.push_back(queue);
        }

        if(meshrq.getCount() > 0) {
            //building the mesh renderqueue
            queue.uniformbuffers[0] = scene->scenebuffer->id;
            queue.uniformbufferbindingpoints[0] = 1;
            queue.data1 = (byte*)&meshrq[0];
            queue.count1 = meshrq.getCount();
            queue.clearcolor = {0.5f, 0.5f, 0.5f, 1.0f};
            queue.commandtype = IME::Rendering::MESH_OBJECT;
            queue.depthtesting = true;
            queue.bufferstoclear = IME::IME_DEPTH_BUFFER | IME::IME_COLOR_BUFFER;
            queue.projection = projection;
            queue.view = view;
            queue.viewheight = framebuffersize.height;
            queue.viewwidth = framebuffersize.width;
            queue.rendertarget = framebuffer;
            queue.updatescene = true;
            queue.viewx = 0;
            queue.viewy = 0;
            queue.dealloc = true;
            rs->renderqueues.push_back(queue);
        }
        
    }
    Entity addNewEntity(const char* tag, SceneData* scene) {
        Entity result = scene->registry.createEntity();

        TagComponent comp;
        comp.tag.set(tag);
        scene->registry.addComponent<TagComponent>(result, comp);
        scene->registry.addComponent<TransformComponent>(result, TransformComponent{identity()});

        return result;
    }



}