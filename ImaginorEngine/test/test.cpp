#include <Windows.h>
#include <datastructures/lists/arrays/FragmentedArrayList.h>
#include <ImaginorEngine/src/memory.h>
#include <core.h>
#include <datastructures/strings/string.h>
#include <datastructures/lists/arrays/ArrayList.h>
#include <datastructures/lists/arrays/ArrayList.h>

#define IME_ASSERT(x) if(!(x)){ __debugbreak(); }

namespace IME::Test {

    byte* allocate(sizeptr size) {
        return (byte*)malloc(size);
    }

    void deallocate(sizeptr size, byte* data) {
        free(data);
    } 

   

    void testArrayList() {

        IME::Data::ArrayList_<int, allocate, deallocate> array;
        array.init(0);

        array.push_back(0);
        array.push_back(1);
        array.push_back(2);
        array.insert(9, 2);
        array.push_back(3);
        array.push_back(4);
        array.push_back(5);
        array.push_back(6);
        array.push_back(7);

        array.destroy();
        

    }
 
    void testmemoryallocator() {

        MemoryArena arena;
        arena.base = (byte*)malloc(IME_MEGABYTE(64));
        arena.size = IME_MEGABYTE(64);
        arena.used = 0;

        MemoryPool pool = createMemoryPool(&arena, arena.size, 256);

        Memory::setGlobal(&pool);

        byte* ptr1 = (byte*)allocateMemory_(&pool, 256);
        byte* ptr2 = (byte*)allocateMemory_(&pool, 256);
        byte* ptr3 = (byte*)allocateMemory_(&pool, 256);
        byte* ptr4 = (byte*)allocateMemory_(&pool, 256);

        byte* ptr5 = (byte*)allocateMemory_(&pool, 256);
        byte* ptr6 = (byte*)allocateMemory_(&pool, 256);
        byte* ptr7 = (byte*)allocateMemory_(&pool, 256);
        byte* ptr8 = (byte*)allocateMemory_(&pool, 256);

        deallocateMemory_(&pool, ptr7, 256);
        deallocateMemory_(&pool, ptr5, 256);

        deallocateMemory_(&pool, ptr6, 256);

        Data::String<Memory::alloc, Memory::dealloc> string;
        Data::String<Memory::alloc, Memory::dealloc> string2;

        Data::ArrayList_<uint32, Memory::alloc, Memory::dealloc> array;
        array.init(0);
        array.push_back(24);
        array.push_back(24);
        array.push_back(24);

        string.set("this is a string");
        string2.set("this is also string");
        string.set("this is a string too");

        array.push_back(24);
        string2.set("another string!");
        array.push_back(24);

        string2.set("this is also string");
        string2.set("another string!");
        string.set("this is a string");
        array.push_back(24);
        string.set("this is a string too");

        string.set("this is a string");
        string2.set("this is also string");
        string.set("this is a string too");

        array.push_back(24);
        string2.set("another string!");
        array.push_back(24);

        string2.set("this is also string");
        string2.set("another string!");
        string.set("this is a string");
        array.push_back(24);
        string.set("this is a string too");

        


    }
}



int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {

    IME::Test::testArrayList();
    

    return 0;
}