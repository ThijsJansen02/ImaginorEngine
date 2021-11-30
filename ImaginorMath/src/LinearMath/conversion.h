#pragma once
#include "mat4.h"
#include "mat3.h"
#include "vec3.h"
#include "eulertransform.h" 

namespace IME { 

    inline EulerTransform 
    eulerTransformFromMat4(const mat4& matrix) {

        EulerTransform result;

        result.scale = { 
            calclength(vec3f{ matrix.rows[0].x, matrix.rows[1].x, matrix.rows[2].x}), 
            calclength(vec3f{ matrix.rows[0].y, matrix.rows[1].y, matrix.rows[2].y}),
            calclength(vec3f{ matrix.rows[0].z, matrix.rows[1].z, matrix.rows[2].z}) };

        result.translation = vec3f{ matrix.rows[0].w, matrix.rows[1].w, matrix.rows[2].w };
        result.rotation = EulerAngelsFromQuaternion(quaternionFromMat4(matrix));

        return result;
    } 

    inline mat4
    transformMat4FromEulerTransform321(const EulerTransform& euler) {

        mat4 result = translationMat4(euler.translation) * rotationMat4({toRadians(euler.rotation.x), toRadians(euler.rotation.y), toRadians(euler.rotation.z)}) * scaleMat4(euler.scale);
        return result;
    }

    vec4f toVec4(const vec2f& in) {
        return {in.x, in.y, 0.0f, 0.0f};
    }

    vec2f toVec2(const vec3f& in) {
        return {in.x, in.y};
    }


}