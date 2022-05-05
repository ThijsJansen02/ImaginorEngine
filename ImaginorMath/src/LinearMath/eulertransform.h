#pragma once
#include "vec3.h"

namespace IME {

    struct EulerTransform {
        vec3f translation;
        union {
            vec3f rotation;
            struct {
                real32 pitch;
                real32 roll;
                real32 yaw;
            };
        };

        vec3f scale;
    };
}