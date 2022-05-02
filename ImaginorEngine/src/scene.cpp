#include "scene.h"
#include "components.h"

namespace IME::Scene {

    void initScene(SceneData* scene) {
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

    void spriteToRenderSet(TransformComponent* tf, SpriteRendererComponent* sr, void* userpointer) {

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

    void pushSceneToRenderSet(Rendering::RenderSet* rs, SceneData* scene, mat4 view, mat4 projection, gl_id framebuffer, vec2<uint32> framebuffersize) {
        
        Rendering::ComplexQuadRQ quadrq;
        quadrq.init(0);

        scene->registry.forEachPair<TransformComponent, SpriteRendererComponent, spriteToRenderSet>((void*)&quadrq);

        Rendering::RenderQueue queue;
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
    Entity addNewEntity(const char* tag, SceneData* scene) {
        Entity result = scene->registry.createEntity();

        TagComponent comp;
        comp.tag.set(tag);
        scene->registry.addComponent<TagComponent>(result, comp);
        scene->registry.addComponent<TransformComponent>(result, TransformComponent{identity()});

        return result;
    }



}