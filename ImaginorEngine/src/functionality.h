#pragma once
#include "intrinsics.h"
#include "memory.h"

namespace IME {

    inline char* copyString(const char* other) {
        sizeptr len = strlen(other) + 1;
        if(len <= 1) {
            return nullptr;
        }
        char* result = (char*)Memory::alloc(len);
        result[len - 1] = '\0';
        copy((byte*)other, (byte*)result, len);
        return result;
    }

    inline void deleteString(char* string) {
        sizeptr len = strlen(string);
        Memory::dealloc(len + 1, (byte*)string);
    }
}