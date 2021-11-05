#include "entt/entity/registry.hpp"
#include <core.h>
#pragma once
#include "rendering/Renderqueue.h"
#include "Components.h"

namespace IME {

    struct Scene {
        entt::registry registry;
    };

    void pushSceneToRenderQueue(Scene* scene, RenderQueue2D* renderqueue) {

        auto view = scene->registry.view<TransformComponent, SpriteRendererComponent>();

        vec2f texcoords[4] = {
            {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}  
        };

        for(auto [entity, transform, sprite] : view.each()) {

            RendererCommand2D command;
            command.color = sprite.color;
            command.shader = sprite.shader;
            command.texture = sprite.texture;
            for(uint32 i = 0; i < 4; i++) {
                command.texcoords[i] = texcoords[i];
            }
            command.transform = transformMat4(transform.transform.position, transform.transform.scale);
            renderqueue->commands.push_back(command);

        }


    }

}