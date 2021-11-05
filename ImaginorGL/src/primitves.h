#pragma once
#include <core.h>
#include <ImaginorPlatform/src/platform.h>
#include "datastructures/lists/arrays/basetypes/Array.h"


namespace IME {

    internal byte* 
    allocator(sizeptr size);

    internal void 
    deallocator(sizeptr size, byte* data);

    template<typename T, sizeptr size> 
    struct PrimitiveSet {
        Data::Array<T, size> data;
        sizeptr firstopenspot;
    };

    internal template<typename T, sizeptr size>
    gl_id
    addNewPrimitive(PrimitiveSet<T, size> *primitiveset, const T& newentry) {

        uint8 testset[sizeof(T)] = {0};
        primitiveset->data[primitiveset->firstopenspot] = newentry;
        uint32 gl_id = primitiveset->firstopenspot;
        for(int i = primitiveset->firstopenspot + 1; i < primitiveset->data.getCapacity(); i++) {
            T primitve = primitiveset->data[i];
            if(!memcmp(&primitve, testset, sizeof(T))) {
                primitiveset->firstopenspot = i;
                return gl_id;
            }
        }
        IME_DEBUG_BREAK();
        return -1;
    }

    internal template<typename T, sizeptr size>
    void
    removePrimitive(PrimitiveSet<T, size> *primitiveset, gl_id) {

        IME_DEBUG_ASSERT_BREAK(gl_id < primitiveset->data.getCapacity(), "gl_id does not exist")

        primitiveset->data[gl_id] = {0};
        if(gl_id < primitiveset->firstopenspot) {
            primitiveset->firstopenspot = gl_id;
        }
    }


}
