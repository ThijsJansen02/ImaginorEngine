#pragma once
#include <core.h>
#include <math.h>
#include <string.h>
#include <stdio.h>

namespace IME
{
    inline real32 squareRootReal32(IME::real32 value) {
        return sqrtf(value);
    }

    inline uint32 absoluteInt32(int32 value) {
        return abs(value);
    }

    inline real32 arcSinReal32(real32 value) {
        return asinf(value);
    }

    inline real32 arcTan2Real32(real32 x, real32 y) {
        return atan2f(x, y);
    }

    inline real32 coTangent(real32 value) {
        return 1.0f / tanf(value);
    }

    inline real32 
    sinReal32(real32 angle) {
        return sinf(angle);
    }

    inline real32 
    cosReal32(real32 angle) {
        return cosf(angle);
    }

    inline int32
    floorReal32(real32 value) {
        return (int32)floorf(value);
    }

    inline int32 
    ceilReal32(real32 value) {
        return (int32)ceilf(value);
    }

    inline int32 roundReal32(real32 value) {
        return (int32)(value + 0.5f);
    }

    inline real32 
    maxReal32(real32 val1, real32 val2) {
        if (val1 >= val2) { return val1; }
        else { return val2; }
    }
    inline real32
    minReal32(real32 val1, real32 val2) {
        if (val1 <= val2) { return val1; }
        else { return val2; }
    }

    inline real32 
    toRadians(real32 degrees) {
        return (degrees / 360) * 2 * PI32;
    }

    inline real32
    powerReal32(real32 base, real32 exp) {
        return powf(base, exp);
    }

    inline int32
    powerInt32(int32 base, int32 exp) {
        int32 result = 1;
        for(int i = 0; i < exp; i++) {
            result *= base;
        }
        return result;
    }

    inline real32 
    absoluteReal32(real32 in) {
        return fabs(in);
    }

    inline void
    copy(byte* src, byte* dst, sizeptr size) {
        /*
        for (int64 i = size - 1; i >= 0; i--) {
            dst[i] = src[i];
        }*/

        memcpy((void*)dst, (void*)src, size);
    } 
}