#pragma once
#include "intrinsics.h"
#include "memory.h"

namespace IME {

    template<typename T1, typename T2>
    struct Pair {
        T1 v1; 
        T2 v2;
    };

    inline char* 
    copyString(const char* other) {
        sizeptr len = strlen(other) + 1;
        if(len <= 1) {
            return nullptr;
        }
        char* result = (char*)Memory::alloc(len);
        result[len - 1] = '\0';
        copy((byte*)other, (byte*)result, len);
        return result;
    }

    inline void 
    deleteString(char* string) {
        sizeptr len = strlen(string);
        Memory::dealloc(len + 1, (byte*)string);
    }
}