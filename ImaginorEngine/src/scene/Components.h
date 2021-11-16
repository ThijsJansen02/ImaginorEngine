#pragma once
#include <ImaginorMath/src/LinearMath/linearmath.h>
#include <datastructures/ecs/ecs.h>

namespace IME {

    struct SpriteRendererComponent {
        gl_id shader;
        gl_id texture;
        vec4f color;

        COMPONENT()
    };

    struct TransformComponent {
        mat4 transform;

        COMPONENT()
    };


}