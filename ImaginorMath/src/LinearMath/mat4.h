#pragma once
#include "core.h"
#include "vec4.h"
#include "mat3.h"

namespace IME {

    struct mat4 {
        union {
            IME::vec4f rows[4];
            IME::real32 data[4 * 4] = {0.0f};
        };
    };

    inline mat4 identity() {
        mat4 result;
        result.data[0 + 0 * 4] = 1.0f;
        result.data[1 + 1 * 4] = 1.0f;
        result.data[2 + 2 * 4] = 1.0f;
        result.data[3 + 3 * 4] = 1.0f;
        return result;
    }

    inline mat4 columnComposeMat4(const vec4f& v1, const vec4f& v2, const vec4f& v3, const vec4f& v4) {

        mat4 result;
        result.rows[0] = {v1.x, v2.x, v3.x, v4.x};
        result.rows[1] = {v1.y, v2.y, v3.y, v4.y};
        result.rows[2] = {v1.z, v2.z, v3.z, v4.z};
        result.rows[3] = {v1.w, v2.w, v3.w, v4.w};
        return result;
    }

    inline mat4 rowComposeMat4(const vec4f& v1, const vec4f& v2, const vec4f& v3, const vec4f& v4) {
        mat4 result;
        result.rows[0] = v1;
        result.rows[1] = v2;
        result.rows[2] = v3;
        result.rows[3] = v4;
        return result;

    }

    inline mat4 perspectiveMat4(real32 fov, real32 near, real32 far, real32 aspectratio) {

        real32 fovr = toRadians(fov);
        mat4 result = {0.0f};
        result.rows[0].x = coTangent(fovr / 2.0f) / aspectratio;

        result.rows[1].y = coTangent(fovr / 2);

        result.rows[2].z = (near + far) / (near - far);
        result.rows[2].w = (2 * near * far) / (near - far);

        result.rows[3].z = -1.0f;

        return result;
    }

    inline mat4 rotationMat4(vec3f angles) {
        mat4 result;

        real32 cosa = cosReal32(angles.z);
        real32 sina = sinReal32(angles.z);

        real32 cosb = cosReal32(angles.y);
        real32 sinb = sinReal32(angles.y);

        real32 cosg = cosReal32(angles.x);
        real32 sing = sinReal32(angles.x);

        result.data[0 + 0 * 4] = cosa * cosb;
        result.data[0 + 1 * 4] = sina * cosb;
        result.data[0 + 2 * 4] = -sinb;

        result.data[1 + 0 * 4] = cosa * sinb * sing - sina * cosg;
        result.data[1 + 1 * 4] = sina * sinb * sing + cosa * cosg;
        result.data[1 + 2 * 4] = cosb * sing;

        result.data[2 + 0 * 4] = cosa * sinb * cosg + sina * sing;
        result.data[2 + 1 * 4] = sina * sinb * cosg - cosa * sing;
        result.data[2 + 2 * 4] = cosb * cosg;

        result.data[3 + 3 * 4] = 1.0f;

        return result;
    }

    inline
    mat3 getInnerMat3(const mat4& input) {

        mat3 result;
        result.rows[0] = vec3f{input.rows[0].x, input.rows[0].y, input.rows[0].z};
        result.rows[1] = vec3f{input.rows[1].x, input.rows[1].y, input.rows[1].z};
        result.rows[2] = vec3f{input.rows[2].x, input.rows[2].y, input.rows[2].z};

        return result;
    }

    inline vec4f toVec4_(const vec3f& v, real32 value = 0.0f) {
        return {v.x, v.y, v.z, value};
    }

    inline 
    mat4 inverseOfOrthagonalMat4(const mat4& mat) {
        IME::real32 x = -mat.rows[0].z;
        IME::real32 y = -mat.rows[1].z;
        IME::real32 z = -mat.rows[2].z;

        mat3 inner = getInnerMat3(mat);

        return columnComposeMat4(toVec4_(inner.rows[0]), toVec4_(inner.rows[1]), toVec4_(inner.rows[2]), vec4f{x, y, z, 1.0f});
    }

    template<typename T> 
    inline mat4 translationMat4(vec3<T> translation) {
        mat4 result = identity();
        result.data[3 + 0 * 4] = translation.x;
        result.data[3 + 1 * 4] = translation.y;
        result.data[3 + 2 * 4] = translation.z;
        return result;
    }

    template<typename T>
    inline mat4 scaleMat4(vec3<T> scale) {
        mat4 result;
        result.data[0 + 0 * 4] = scale.x;
        result.data[1 + 1 * 4] = scale.y;
        result.data[2 + 2 * 4] = scale.z;
        result.data[3 + 3 * 4] = 1.0f;
        return result;
    }

    template<typename T>
    inline mat4 
    transformMat4(vec3<T> translation, vec3<T> scale) {

        mat4 result;
        result.data[0 + 0 * 4] = scale.x;
        result.data[1 + 1 * 4] = scale.y;
        result.data[2 + 2 * 4] = scale.z;
        result.data[3 + 3 * 4] = 1.0f;

        result.data[3 + 0 * 4] = translation.x;
        result.data[3 + 1 * 4] = translation.y;
        result.data[3 + 2 * 4] = translation.z;

        return result;
    }

    inline mat4 
    OrthographicMat4(real32 left, real32 right, real32 bottom, real32 top, real32 far, real32 near)
    {
        real32 width = right - left;
        real32 height = top - bottom;
        real32 thickness = far - near;

        real32 middleX = (left + right) / 2.0f;
        real32 middleY = (top + bottom) / 2.0f;
        real32 middleZ = (near + far) / 2.0f;

        mat4 result = {0};
        result.data[0 + 0 * 4] = 2.0f / width;
        result.data[1 + 1 * 4] = 2.0f / height;
        result.data[2 + 2 * 4] = 2.0f / thickness;
        result.data[3 + 3 * 4] = 1.0f;

        result.data[3 + 0 * 4] = -middleX * (2.0f / width);
        result.data[3 + 1 * 4] = -middleY * (2.0f / height);
        result.data[3 + 2 * 4] = -middleZ * (2.0f / height);

        return result;
    }

    inline mat4 
    operator*(const mat4& left, const mat4& right) {

        mat4 result;
        for(uint32 y = 0; y < 4; y++) {
            for(uint32 x = 0; x < 4; x++) {

                result.data[x + y * 4] =    
                        left.data[0 + y * 4] * right.data[x + (0) * 4] + 
                        left.data[1 + y * 4] * right.data[x + (1) * 4] + 
                        left.data[2 + y * 4] * right.data[x + (2) * 4] + 
                        left.data[3 + y * 4] * right.data[x + (3) * 4];
            }
        }
        return result;
    }

    template<typename T>
    inline vec4<T> 
    operator*(const mat4& left, const vec4<T>& right) {

        vec4<T> result;
        result.x = left.data[0 + 0 * 4] * right.x + left.data[1 + 0 * 4] * right.y + left.data[2 + 0 * 4] * right.z + left.data[3 + 0 * 4] * right.w;
        result.y = left.data[0 + 1 * 4] * right.x + left.data[1 + 1 * 4] * right.y + left.data[2 + 1 * 4] * right.z + left.data[3 + 1 * 4] * right.w;
        result.z = left.data[0 + 2 * 4] * right.x + left.data[1 + 2 * 4] * right.y + left.data[2 + 2 * 4] * right.z + left.data[3 + 2 * 4] * right.w;
        result.w = left.data[0 + 3 * 4] * right.x + left.data[1 + 3 * 4] * right.y + left.data[2 + 3 * 4] * right.z + left.data[3 + 3 * 4] * right.w;
        return result;
    }

    template<typename T>
    inline vec3<T> 
    operator*(const mat4& left, const vec3<T>& right) {

        vec3<T> result;
        result.x = left.data[0 + 0 * 4] * right.x + left.data[1 + 0 * 4] * right.y + left.data[2 + 0 * 4] * right.z + left.data[3 + 0 * 4] * 1.0f;
        result.y = left.data[0 + 1 * 4] * right.x + left.data[1 + 1 * 4] * right.y + left.data[2 + 1 * 4] * right.z + left.data[3 + 1 * 4] * 1.0f;
        result.z = left.data[0 + 2 * 4] * right.x + left.data[1 + 2 * 4] * right.y + left.data[2 + 2 * 4] * right.z + left.data[3 + 2 * 4] * 1.0f;
        return result;
    }
}
