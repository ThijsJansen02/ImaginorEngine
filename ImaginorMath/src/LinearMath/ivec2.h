#pragma once
#include <core.h>

namespace IME {

    struct ivec2
    {
        ivec2(int32 x, int32 y) : x(x), y(y) {}
        ivec2() = default;

        union 
        {
            struct 
            {
                int32 x;
                int32 y;
            };
            int32 data[2];
        };
        void operator+=(const ivec2& right) {
            x += right.x;
            y += right.y;
        }
    };

    inline ivec2 operator*(const ivec2 &left, const ivec2 &right)
    {
        return {left.x * right.x, left.y * right.y};
    }

    inline ivec2 operator/(const ivec2 &left, const ivec2 &right)
    {
        return {left.x / right.x, left.y / right.y};
    }

    inline ivec2 operator+(const ivec2 &left, const ivec2 &right)
    {
        return {left.x + right.x, left.y + right.y};
    }

    inline ivec2 operator-(const ivec2 &left, const ivec2 &right)
    {
        return {left.x - right.x, left.y - right.y};
    }

    inline ivec2 operator*(int32 left, const ivec2 &right)
    {
        return {left * right.x, left * right.y};
    }

    inline ivec2 operator/(const ivec2 &left, int32 right)
    {
        return {left.x / right, left.y / right};
    }

    inline ivec2 operator+(ivec2 left, int32 right) {
        return { left.x + right, left.y + right };
    }

    int32 dotProduct(const ivec2 &left, const ivec2 &right)
    {
        return left.x * right.x + left.y * right.y;
    }
}

