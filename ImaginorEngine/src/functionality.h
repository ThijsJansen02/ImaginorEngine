#pragma once
#include "intrinsics.h"
#include "memory.h"

namespace IME {

    inline char* copyString(const char* other) {
        sizeptr len = strlen(other) + 1;
        char* result = (char*)Memory::alloc(len);
        copy((byte*)other, (byte*)result, len);
        return result;
    }

}