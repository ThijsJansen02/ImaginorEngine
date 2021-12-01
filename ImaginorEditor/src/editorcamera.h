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

        IME_DEBUG_ASSERT_BREAK(lengthSquared(camera.forward) == 1.0f && lengthSquared(worldup) == 1.0f, "forward is not normalized")

        Quaternion yaw = quaternionFromAngleVector(toRadians(camera.yaw), -1.0f * camera.forward);
        yaw.vector = normalize(yaw.vector);


        vec3f right = applyQuatRotationToVec3(crossProduct(camera.forward, worldup), yaw);

        vec3f camera_up = crossProduct(right, camera.forward);

        //mat4 view = translateMat3( camera.position)
        mat4 view = columnComposeMat4(toVec4(right), toVec4(camera_up), toVec4(-1.0f * camera.forward), toVec4(-1.0f * camera.position, 1.0f));

        return camera.projection * view;
    }
}