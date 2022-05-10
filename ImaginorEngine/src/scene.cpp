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

    void meshToRenderSet(MeshRenderComponent* mr, TransformComponent* tf, Entity entity,  void* userpointer) {

        Rendering::MeshRQ* meshrq = (Rendering::MeshRQ*)userpointer;
        
        Rendering::MeshObjectCommand command;
        command.shader = mr->shader_id;
        command.rbo = mr->rbo_id;
        command.texture = mr->texture_id;        
        command.transform = tf->transform;
        command.id = entity.index;
        meshrq->push_back(command);
    }

    void spriteToRenderSet(SpriteRendererComponent* sr, TransformComponent* tf, Entity entity, void* userpointer) {

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

    void pushSceneToRenderSet(Rendering::RenderSet* rs, SceneData* scene, mat4 view, mat4 projection, Assets::FrameBuffer* framebuffer, vec2<uint32> framebuffersize, PlatformInterface& platform) {
        
        Rendering::ComplexQuadRQ quadrq;
        Rendering::MeshRQ meshrq;

        mat4 viewprojection = projection * view;

        platform.gfx.bindubo(scene->scenebuffer->id, 0, 0, 0);
        platform.gfx.ubobuffersubdata((byte*)&viewprojection, sizeof(mat4), 0);

        platform.gfx.fbo_bind(framebuffer->id);
        platform.gfx.clear(IME_COLOR_BUFFER | IME_DEPTH_BUFFER);

        uint32 value = 0xFFFFFFFF;
        platform.gfx.texture_clear((byte*)&value, ((Assets::Texture*)framebuffer->colorattachements[1].data)->id);

        quadrq.init(0);
        meshrq.init(0);

        scene->registry.forEachPair<SpriteRendererComponent, TransformComponent, spriteToRenderSet>((void*)&quadrq);
        scene->registry.forEachPair<MeshRenderComponent, TransformComponent, meshToRenderSet>((void*)&meshrq);

        Rendering::RenderQueue queue;

            queue.data1 = (byte*)&quadrq[0];
            queue.count1 = quadrq.getCount();
            queue.clearcolor = {0.5f, 0.5f, 0.5f, 1.0f};
            queue.commandtype = IME::Rendering::COMPLEX_QUAD;
            queue.depthtesting = true;
            queue.bufferstoclear = 0;
            queue.projection = projection;
            queue.view = view;
            queue.viewheight = framebuffersize.height;
            queue.viewwidth = framebuffersize.width;
            queue.rendertarget = framebuffer->id;
            queue.updatescene = true;
            queue.viewx = 0;
            queue.viewy = 0;
            queue.dealloc = true;
            rs->renderqueues.push_back(queue);

            //building the mesh renderqueue
            queue.uniformbuffers[0] = scene->scenebuffer->id;
            queue.uniformbufferbindingpoints[0] = 1;
            queue.data1 = (byte*)&meshrq[0];
            queue.count1 = meshrq.getCount();
            queue.clearcolor = {0.5f, 0.5f, 0.5f, 1.0f};
            queue.commandtype = IME::Rendering::MESH_OBJECT;
            queue.depthtesting = true;
            queue.bufferstoclear = 0;
            queue.projection = projection;
            queue.view = view;
            queue.viewheight = framebuffersize.height;
            queue.viewwidth = framebuffersize.width;
            queue.rendertarget = framebuffer->id;
            queue.updatescene = true;
            queue.viewx = 0;
            queue.viewy = 0;
            queue.dealloc = true;
            rs->renderqueues.push_back(queue);
    }

    struct tcdata {
        uint32 entityindex;
        mat4 transform;
    };

    struct mrdata {
        uint32 entityindex;
        uuid meshasset;
        uuid textureasset;
    };

    struct tagdata {
        uint32 Entity;
        sizeptr tagoffset;
    };

    struct sceneheader {

        uint32 entitiesoffset;
        uint32 entitiescount;
        
        uint32 tcoffset;
        uint32 tccount;

        uint32 mroffset;
        uint32 mrcount;

        uint32 tagoffset;
        uint32 tagcount;
    };

    void saveScene(const char* filepath, SceneData* scene, PlatformInterface& platform) {

        


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