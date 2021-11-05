#include <ImaginorMath/src/LinearMath/linearmath.h>
#include <ImaginorPlatform/src/imegs.h>

namespace IME {

    struct EulerTransforms {
        vec3f position;
        vec3f scale;
        vec3f rotation;
    };

    struct TransformComponent {
        EulerTransforms transform; 
    };

    struct SpriteRendererComponent {
        vec4f color;
        gl_id texture;
        gl_id shader;
    };


}