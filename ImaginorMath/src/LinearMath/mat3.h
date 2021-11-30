#pragma once
#include <core.h>
#include "vec2.h"
#include <intrinsics.h>

namespace IME {

    struct mat3
    {   
        union {
            real32 data[3 * 3] = {0.0f};
            vec3f rows[3];
        };
    };

    inline mat3 operator*(const mat3 &left, const mat3 &right)
    {
        mat3 result;

        for (uint32 y = 0; y < 3; y++)
        {
            for (uint32 x = 0; x < 3; x++)
            {
                result.data[x + y * 3] = left.data[0 + y * 3] * right.data[x + (0) * 3] + left.data[1 + y * 3] * right.data[x + (1) * 3] + left.data[2 + y * 3] * right.data[x + (2) * 3];
            }
        }
        return result;
    }

    inline vec2f operator*(const mat3 &right, const vec2f &left)
    {
        vec2f result;
        result.x = right.data[0 + 0 * 3] * left.x + right.data[1 + 0 * 3] * left.y + right.data[2 + 0 * 3];
        result.y = right.data[0 + 1 * 3] * left.x + right.data[1 + 1 * 3] * left.y + right.data[2 + 1 * 3];
        return result;
    }

    inline vec3f operator*(const mat3 &right, const vec3f &left)
    {
        vec3f result;
        result.x = right.data[0 + 0 * 3] * left.x + right.data[1 + 0 * 3] * left.y + right.data[2 + 0 * 3] * left.z;
        result.y = right.data[0 + 1 * 3] * left.x + right.data[1 + 1 * 3] * left.y + right.data[2 + 1 * 3] * left.z;
        result.z = right.data[0 + 2 * 3] * left.x + right.data[1 + 2 * 3] * left.y + right.data[2 + 2 * 3] * left.z;
        return result;
    }

    inline mat3 rotateMat3(real32 rotation) {
        mat3 result;
        real32 costheta = cosReal32(rotation);
        real32 sinetheta = sinReal32(rotation);

        result.data[0 + 0 * 3] = costheta;
        result.data[1 + 0 * 3] = -sinetheta;
        result.data[0 + 1 * 3] = sinetheta;
        result.data[1 + 1 * 3] = costheta;

        result.data[2 + 2 * 3] = 1.0f;
        
        return result;
    }

    inline mat3 transformMat3(const vec2f& scaling, const vec2f& translation) {

        mat3 result;

        result.data[0 + 0 * 3] = scaling.x;
        result.data[1 + 1 * 3] = scaling.y;
        result.data[2 + 2 * 3] = 1.0f;

        result.data[2 + 0 * 3] = translation.x;
        result.data[2 + 1 * 3] = translation.y;

        return result;
    }

    inline mat3 translateMat3(const vec2f& translation) {
        mat3 result;

        result.data[0 + 0 * 3] = 1.0f;
        result.data[1 + 1 * 3] = 1.0f;
        result.data[2 + 2 * 3] = 1.0f;

        result.data[2 + 0 * 3] = translation.x;
        result.data[2 + 1 * 3] = translation.y;
        return result;
    }

    inline mat3 createOrthographicMat3(real32 left, real32 right, real32 bottom, real32 top)
    {
        real32 width = right - left;
        real32 height = top - bottom;

        real32 middleX = (left + right) / 2.0f;
        real32 middleY = (top + bottom) / 2.0f;

        mat3 result;
        result.data[0 + 0 * 3] = 2.0f / width;
        result.data[1 + 1 * 3] = 2.0f / height;
        result.data[2 + 2 * 3] = 1.0f;

        result.data[2 + 0 * 3] = -middleX * (2.0f / width);
        result.data[2 + 1 * 3] = -middleY * (2.0f / height);

        return result;
    }


}