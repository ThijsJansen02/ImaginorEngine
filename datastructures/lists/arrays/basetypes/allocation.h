#include<core.h>

namespace IME::Data {

    byte* std_allocator(sizeptr size) {
        return (byte*)malloc(size);
    }

    void std_deallocator(sizeptr size, byte* data) {
        free(data);
    }

}