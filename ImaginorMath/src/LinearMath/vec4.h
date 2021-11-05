#pragma once

#include <core.h>
#include <math.h>

namespace IME
{   
    template<typename T>
    struct vec2;

    template<typename T>
    struct vec4
    {
        T x, y, z, w;
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
        void operator+=(const vec4& right) {
            x += right.x;
            y += right.y;
            z += right.z;
            w += right.w;
        }
    };

    template<typename T, typename R>
    inline vec4<T> operator*(const vec4<T> &left, const vec4<R> &right)
    {
        return {left.x * right.x, left.y * right.y, left.z * right.z, left.w * right.w};
    }

    template<typename T, typename R>
    inline vec4<T> operator/(const vec4<T> &left, const vec4<R> &right)
    {
        return {left.x / right.x, left.y / right.y, left.z / right.z, left.w / right.w};
    }

    template<typename T, typename R>
    inline vec4<T> operator+(const vec4<T> &left, const vec4<R> &right)
    {
        return { left.x + right.x, left.y + right.y, left.z + right.z, left.w + right.w };
    }

    template<typename T, typename R>
    inline vec4<T> operator-(const vec4<T> &left, const vec4<R> &right)
    {
        return {left.x - right.x, left.y - right.y, left.z - right.z, left.w - right.w};
    }

    template<typename T>
    inline vec4<T> operator*(T left, const vec4<T> &right)
    {
        return {left * right.x, left * right.y, left * right.z, left * right.w};
    }

    template<typename T>
    inline vec4<T> operator*(const vec4<T> &left, T right)
    {
        return {left.x * right, left.y * right, left.z * right, left.w * right};
    }

    template<typename T>
    inline vec4<T> operator/(const vec4<T> &left, T right)
    {
        return {left.x / right, left.y / right, left.z / right, left.w / right};
    }


    template<typename T>
    inline vec4<T> operator+(vec4<T> left, T right) {
        return { left.x + right, left.y + right, left.z + right, left.w + right };
    }
    
    template<typename T> 
    inline vec4<T> vec2Tovec4(const vec2<T>& other) {
        return { other.x, other.y, (T)0, (T)0 };
    } 

    template<typename T>
    inline T dotProduct(const vec4<T> &left, const vec4<T> &right)
    {
        return left.x * right.x + left.y * right.y + left.z * right.z + left.w * right.w;
    }

    template<typename T> 
    inline T calclength(const vec4<T>& vector) {
        return squareRootReal32(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z + vector.w * vector.w);
    }

#ifdef CROSSVEC4
    template<typename T>
    T crossProduct(const vec4<T> &left, const vec4<T> &right) {
        vec4<T> result;

        result.x = left.y * right.z - left.z * right.y;
        result.y = left.z * right.x - left.x * right.z;
        result.z = left.x * right.y - left.y * right.x;

        return result;

    }
#endif

    template<typename T>
    inline vec4<T> normalize(const vec4<T>& vector) {
        return vector / calclength(vector);
    }

    typedef vec4<real32> vec4f;
}