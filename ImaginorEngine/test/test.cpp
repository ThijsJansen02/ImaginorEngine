#include <Windows.h>
#include <datastructures/lists/arrays/FragmentedArrayList.h>
#include <ImaginorEngine/src/memory.h>
#include <core.h>
#include <datastructures/strings/string.h>
#include <datastructures/lists/arrays/ArrayList.h>

#define IME_ASSERT(x) if(!(x)){ __debugbreak(); }

namespace IME::Test {

    byte* allocate(sizeptr size) {
        return (byte*)malloc(size);
    }

    void deallocate(sizeptr size, byte* data) {
        free(data);
    } 

    template<typename T>
    using FragmentedArrayList = Data::FragmentedArrayList<T, allocate, deallocate>;

    void fragmentedArrayListTest() {
        FragmentedArrayList<uint32> list = FragmentedArrayList<uint32>::createFragmentedArrayList(1);
        sizeptr index1 = list.add(2);
        sizeptr index2 = list.add(2);
        sizeptr index3 = list.add(2);
        sizeptr index4 = list.add(2);
        sizeptr index5 = list.add(2);
        sizeptr index6 = list.add(2);
        sizeptr index7 = list.add(2);
        sizeptr index8 = list.add(2);
        sizeptr index9 = list.add(2);
        
        IME_ASSERT(list.calcCount() == 9)
        IME_ASSERT(list.getCount() == 9)

        list.remove(index4);
        list.remove(index5);

        IME_ASSERT(list.getCount() == 9)
        IME_ASSERT(list.calcCount() == 7)
        IME_ASSERT(list.m_Firstopenspot == 3)

        list.remove(index1);
        list.remove(index2);

        IME_ASSERT(list.m_Firstopenspot == 0)
        list.add(2);
        IME_ASSERT(list.m_Firstopenspot == 1)

        list.add(2);
        IME_ASSERT(list.m_Firstopenspot == 3)

        IME_ASSERT(list.getCount() == 9)
        IME_ASSERT(list.calcCount() == 7)
        IME_ASSERT(list.m_Firstopenspot == 3)

        list.remove(index7);
        list.remove(index8);

        IME_ASSERT(list.getCount() == 9)
        IME_ASSERT(list.calcCount() == 5)
        IME_ASSERT(list.m_Firstopenspot == 3)

        list.remove(index9);
        IME_ASSERT(list.getCount() == 6)
        IME_ASSERT(list.calcCount() == 4)
        IME_ASSERT(list.m_Firstopenspot == 3)

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

        Data::ArrayList_<uint32, Memory::alloc, Memory::dealloc> array = Data::ArrayList_<uint32, Memory::alloc, Memory::dealloc>::create(0);
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

    IME::Test::fragmentedArrayListTest();
    IME::Test::testmemoryallocator();

    return 0;
}