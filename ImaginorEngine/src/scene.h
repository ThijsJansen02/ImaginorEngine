#include <datastructures/ecs/ecs.h>
#include "rendering.h"
#include "memory.h"
#include "components.h"

namespace IME::Scene {

    typedef Data::Registry<2048, 20, Memory::alloc, Memory::dealloc> SceneRegistry;
    typedef Data::Entity Entity;

    struct SceneData {
        SceneRegistry registry;
    };


    void initScene(SceneData* scene);
    void pushSceneToRenderSet(Rendering::RenderSet* rs, SceneData* scene, mat4 view, mat4 projection, gl_id framebuffer, vec2<uint32> framebuffersize);
    Entity addNewEntity(const char* tag, SceneData* scene);

    template<typename comp>
    comp* getComponent(Entity entity, SceneData* scene) {
        return &scene->registry.getComponent<comp>(entity);
    }

    template<typename comp>
    bool32 hasComponent(Entity entity, SceneData* scene) {
        return scene->registry.hasComponent<comp>(entity);
    }

    template<typename comp>
    void addComponentToEntity(Entity entity, comp component, SceneData* scene) {
        scene->registry.addComponent<comp>(entity, component);
    }

    template<typename comp>
    void removeComponentFromEntity(Entity entity, SceneData* scene) {
        scene->registry.removeComponent<comp>(entity);
    }
}