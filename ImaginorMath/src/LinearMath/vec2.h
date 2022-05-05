#pragma once

#include <core.h>
#include <math.h>

namespace IME
{
    template<typename T>
    struct vec2
    {
        vec2(T x, T y) : x(x), y(y) {}
        vec2() = default;
        union {
            struct {
                T x;
                T y;
            };
            struct {
                T width;
                T height;
            };
        };

        void operator+=(const vec2& right) {
            x += right.x;
            y += right.y;
        }
    };

    template<typename T, typename R>
    inline vec2<T> operator*(const vec2<T> &left, const vec2<R> &right)
    {
        return {left.x * right.x, left.y * right.y};
    }

    template<typename T, typename R>
    inline bool operator==(const vec2<T> &left, const vec2<R> &right)
    {
        return left.x == right.x && left.y == right.y;
    }

    template<typename T, typename R>
    inline vec2<T> operator/(const vec2<T> &left, const vec2<R> &right)
    {
        return {left.x / right.x, left.y / right.y};
    }

    template<typename T, typename R>
    inline vec2<T> operator+(const vec2<T> &left, const vec2<R> &right)
    {
        return {left.x + right.x, left.y + right.y};
    }

    template<typename T, typename R>
    inline vec2<T> operator-(const vec2<T> &left, const vec2<R> &right)
    {
        return {left.x - right.x, left.y - right.y};
    }

    template<typename T>
    inline vec2<T> operator*(T left, const vec2<T> &right)
    {
        return {left * right.x, left * right.y};
    }

    template<typename T>
    inline vec2<T> operator/(const vec2<T> &left, T right)
    {
        return {left.x / right, left.y / right};
    }

    template<typename T>
    inline vec2<T> operator+(vec2<T> left, T right) {
        return { left.x + right, left.y + right };
    }
    
template<typename T> 
    inline vec2<T> absolute(const vec2<T>& vec) {
        return vec2<T>{absoluteReal32(vec.x), absoluteReal32(vec.y)};
    }

    template<typename T>
    inline T lengthSquared(const vec2<T> &vec)
    {
        return vec.x * vec.x + vec.y * vec.y;
    }

    template<typename T>
    inline vec2<T> normalize(const vec2<T>& vec) {
        return 1 / squareRootReal32(lengthSquared(vec)) * vec;
    }

    template<typename T>
    inline T dotProduct(const vec2<T> &left, const vec2<T> &right)
    {
        return left.x * right.x + left.y * right.y;
    }

    template<typename T>
    inline vec2<T> perpendicularCounterClockwise(vec2<T> vec)
    {
        return vec2<T>{-vec.y, vec.x};
        
    }

    typedef vec2<real32> vec2f;
}