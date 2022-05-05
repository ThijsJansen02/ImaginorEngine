#pragma once
#include <ImaginorMath/src/LinearMath/linearmath.h>
#include <datastructures/ecs/ecs.h>
#include <datastructures/strings/string.h>
#include "assets.h"
#include "script.h"

namespace IME::Scene {

    typedef Data::String<Memory::alloc, Memory::dealloc> String;

    struct TagComponent {
        String tag;

        COMPONENT()
    };

    struct TransformComponent {
        mat4 transform;

        COMPONENT()
    };

    struct SpriteRendererComponent {
        gl_id shader;
        gl_id textureid;
        Assets::Texture* texture;
        vec4f color;

        COMPONENT()
    };

    struct CameraComponent {

        mat4 projection;

        COMPONENT()
    };

    struct ScriptComponent {
        COMPONENT()

        IME::alloc_script* alloc;
        IME::dealloc_script* dealloc;
        IME::script_init* init;
        IME::script_on_update* onupdate;
        IME::script_on_event* onevent;

        void* data;
    };

    struct MeshRenderComponent {
        COMPONENT()

        Assets::Mesh* mesh;
        gl_id rbo_id;
        gl_id texture_id;
        gl_id shader_id;
    };
}