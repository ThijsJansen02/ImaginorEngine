#pragma once
#include <ImaginorMath/src/LinearMath/linearmath.h>
#include <datastructures/ecs/ecs.h>
#include <datastructures/strings/string.h>
#include "assets.h"

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
}