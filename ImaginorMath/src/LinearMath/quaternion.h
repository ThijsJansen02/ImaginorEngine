#pragma once
#include "vec4.h"
#include "mat4.h"
#include <core.h>

namespace IME {

    struct Quaternion {

        union
        {
            vec4f vector;
            real32 data[4];
            struct {
                real32 i;
                real32 j;
                real32 k; 
                real32 real; 
            };
            struct {
                real32 x;
                real32 y;
                real32 z; 
                real32 real; 
            };
        };
    };

    inline vec3f
    EulerAngelsFromQuaternion(const Quaternion& q) {
        return { 
            (360.0f / (2.0f * PI32)) * arcTan2Real32(2 * (q.real * q.i + q.j * q.k), 1-2*(q.i * q.i + q.j * q.j)),
            (360.0f / (2.0f * PI32)) * arcSinReal32(2 * (q.real * q.j - q.k * q.i)),
            (360.0f / (2.0f * PI32)) * arcTan2Real32(2 * (q.real * q.k + q.i * q.j), 1-2*(q.j * q.j + q.k * q.k))
        };
    }


    inline Quaternion 
    quaternionFromMat4(const mat4& matrix) {

        vec3f c1 = {matrix.rows[0].x, matrix.rows[1].x, matrix.rows[2].x };
        vec3f c2 = {matrix.rows[0].y, matrix.rows[1].y, matrix.rows[2].y };
        vec3f c3 = {matrix.rows[0].z, matrix.rows[1].z, matrix.rows[2].z };

        c1 = normalize(c1);
        c2 = normalize(c2);
        c3 = normalize(c3);

        Quaternion result;
        result.real = squareRootReal32(1.0f + c1.x + c2.y + c3.z) / 2.0f;
        result.i = (c2.z - c3.y) / (4 * result.real);
        result.j = (c3.x - c1.z) / (4 * result.real);
        result.k = (c1.y - c2.x) / (4 * result.real);

        return result;
    }

    inline Quaternion
    quaternionFromAngleVector(real32 angle, vec3f vector) {

        Quaternion result;

        real32 ca = cosReal32(angle);
        real32 sa = sinReal32(angle);

        result.real = ca;
        result.i = vector.x * sa;
        result.j = vector.y * sa;
        result.k = vector.z * sa;

        return result;

    }

    inline Quaternion 
    operator*(const Quaternion& left, const Quaternion& right) {
        return { 
            left.real * right.real - left.i * right.i - left.j * right.j - left.k * right.k,
            left.real * right.i + left.i * right.real + left.j * right.k - left.k * right.j,
            left.real * right.j - left.i * right.k + left.j * right.real + left.k * right.i,
            left.real * right.k + left.i * right.j - left.j * right.i + left.k * right.real };
    };

    inline 
    mat4 quaternionToMat4(const Quaternion& quat) {

        mat4 result;

        real32 x = quat.x;
        real32 y = quat.y;
        real32 z = quat.z;
        real32 s = quat.real;

        result.rows[0] = { 1 - 2*y*y - 2*z*z, 2*x*y - 2*s*z, 2*x*z + 2*s*y, 0.0f };
        result.rows[1] = { 2*x*y + 2*s*z, 1 - 2*x*x - 2*z*z, 2*y*z - 2*s*x, 0.0f };
        result.rows[2] = { 2*x*z - 2*s*y, 2*y*z + 2*s*x, 1 - 2*x*x - 2*y*y, 0.0f };
        result.rows[3] = { 0.0f, 0.0f, 0.0f, 1.0f };
        return result;
    }

    
}
