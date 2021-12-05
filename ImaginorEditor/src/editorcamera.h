#pragma once
#include <ImaginorMath/src/LinearMath/linearmath.h>

namespace IME {

    struct EditorCamera {
        
        real32 yaw;
        vec3f forward;
        vec3f position;

        mat4 projection;    
    };

    mat4 calculateViewProjection(const EditorCamera& camera, vec3f worldup) {

        IME_DEBUG_ASSERT_BREAK(lengthSquared(worldup) == 1.0f, "forward is not normalized")

        Quaternion roll = quaternionFromAngleVector(toRadians(camera.yaw), camera.forward);
        roll.vector = normalize(roll.vector);

        vec3f forward = normalize(-1.0f * camera.forward);

        vec3f right = normalize(applyQuatRotationToVec3(crossProduct(worldup, forward), roll));
        vec3f camera_up = normalize(crossProduct(forward, right));
        mat4 view = rowComposeMat4(toVec4(right), toVec4(camera_up), toVec4(forward), {0.0f, 0.0f, 0.0f, 1.0f}) * translationMat4(-1.0f * camera.position);

        return camera.projection * view;
    }
}