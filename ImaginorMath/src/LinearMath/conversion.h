#pragma once
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

    inline vec4f toVec4(const vec2f& in, real32 z = 0.0f, real32 w = 0.0f) {
        return {in.x, in.y, z, w};
    }

    inline vec2f toVec2(const vec3f& in) {
        return {in.x, in.y};
    }

    inline vec4f toVec4(const vec3f& in, real32 w = 0.0f) {
        return {in.x, in.y, in.z, w};
    }

    inline Quaternion toQuat(const vec3f& in, real32 real = 0.0f) {
        return {in.x, in.y, in.z, real};
    }

    inline vec3f toVec3(const Quaternion& quat) {
        return {quat.x, quat.y, quat.z};
    }

    inline vec3f toVec3(const vec2f& in, real32 z = 0.0f) {
    return {in.x, in.y, z};
    }

    inline vec2f toVec2f(vec2<uint32> values) {
        return {(real32)values.x, (real32)values.y};
    }

    inline vec3f
    applyQuatRotationToVec3(const vec3f& vector, const Quaternion& quat) {

        Quaternion quatvector = toQuat(vector);
        Quaternion conjugated = conjugate(quat);
        return toVec3(quat * quatvector * conjugated);
    }

}