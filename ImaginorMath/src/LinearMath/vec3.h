#pragma once

#include <core.h>
#include <math.h>

namespace IME
{
    template<typename T>
    struct vec2;

    template<typename T>
    struct vec3
    {
        T x, y, z;
/*
        union {
            struct {
                T x, y, z;
            };
            struct {
                T r, g, b;
            };
            struct {
                T data[3];
            }
        }
*/
        void operator+=(const vec3& right) {
            x += right.x;
            y += right.y;
            z += right.z;
        }
    };

    template<typename T, typename R>
    inline vec3<T> operator*(const vec3<T> &left, const vec3<R> &right)
    {
        return {left.x * right.x, left.y * right.y, left.z * right.z};
    }

    template<typename T, typename R>
    inline vec3<T> operator/(const vec3<T> &left, const vec3<R> &right)
    {
        return {left.x / right.x, left.y / right.y, left.z / right.z};
    }

    template<typename T, typename R>
    inline vec3<T> operator+(const vec3<T> &left, const vec3<R> &right)
    {
        return {left.x + right.x, left.y + right.y, left.z + right.z};
    }

    template<typename T, typename R>
    inline vec3<T> operator-(const vec3<T> &left, const vec3<R> &right)
    {
        return {left.x - right.x, left.y - right.y, left.z - right.z};
    }

    template<typename T>
    inline vec3<T> operator*(T left, const vec3<T> &right)
    {
        return {left * right.x, left * right.y, left * right.z};
    }

    template<typename T>
    inline vec3<T> operator*(const vec3<T> &left, T right)
    {
        return {left.x * right, left.y * right, left.z * right};
    }

    template<typename T>
    inline vec3<T> operator/(const vec3<T> &left, T right)
    {
        return {left.x / right, left.y / right, left.z / right};
    }


    template<typename T>
    inline vec3<T> operator+(vec3<T> left, T right) {
        return { left.x + right, left.y + right, left.z + right };
    }
    
    template<typename T> 
    inline vec3<T> vec2ToVec3(const vec2<T>& other) {
        return { other.x, other.y, (T)0 };
    } 

    template<typename T>
    inline T dotProduct(const vec3<T> &left, const vec3<T> &right)
    {
        return left.x * right.x + left.y * right.y + left.z * right.z;
    }

    template<typename T> 
    inline T calclength(const vec3<T>& vector) {
        return squareRootReal32(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
    }

    template<typename T>
    inline vec3<T> crossProduct(const vec3<T> &left, const vec3<T> &right) {
        vec3<T> result;

        result.x = left.y * right.z - left.z * right.y;
        result.y = left.z * right.x - left.x * right.z;
        result.z = left.x * right.y - left.y * right.x;

        return result;

    }

    template<typename T>
    inline vec3<T> normalize(const vec3<T>& vector) {
        return vector / calclength(vector);
    }

    typedef vec3<real32> vec3f;
}