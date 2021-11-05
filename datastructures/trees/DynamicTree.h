#include <core.h>
#include <datastructures/lists/arrays/ArrayList.h>

namespace IME::Data {

    typedef byte* allocator(sizeptr size);
    typedef void deallocator(sizeptr size, byte* data);

    template<typename T, allocator alloc, deallocator dealloc>
    class DynamicTree {
    public:

        struct Branch {
            T data;
            ArrayList_<Branch*, alloc, dealloc> branches;
        };

        Branch root;
    };
}