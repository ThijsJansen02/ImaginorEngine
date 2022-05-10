#pragma once
#include <core.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <cstdlib>

namespace IME
{
    inline bool isDigit(const char* s1) {
        bool32 comma = false;
        while(*s1) {
            if(*s1 == '.') {
                if(comma) {
                    return false;
                } else {
                    comma = true;
                    s1++;
                    continue;
                }
            }
            if(*s1 < '0' || *s1 > '9') {
                return false;
            }
            s1++;
        }   
        return true;
    }

    inline bool compareStrings(const char* s1, const char* s2) {
        int32 result = strcmp(s1, s2);
        return !*(bool*)&result;
    }

    inline void int32ToString(char* buffer, int32 value, sizeptr s) {
        sprintf_s(buffer, s, "%d", value);
    }

    inline void uint32ToString(char* buffer, uint32 value, sizeptr s) {
        sprintf_s(buffer, s, "%u", value);
    }

    inline int32 stringToInt32(const char* string) {
        return atoi(string);
    }

    inline uint32 stringToUint32(const char* string) {
        return atoi(string);
    }

    inline real32
    stringToReal32(const char* str) {
        return strtof(str, nullptr);
    } 

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
        
        //for (int64 i = ((int64)size) - 1; i >= 0; i--) {
        //    dst[i] = src[i];
        //}

        memcpy((void*)dst, (void*)src, size);
    } 

    inline void
    copyforwards(byte* src, byte* dst, sizeptr size) {
        
        for (int64 i = 0; i < size; i++) {
            dst[i] = src[i];
        }

        //memcpy((void*)dst, (void*)src, size);
    } 
}