#include <Windows.h>
#include <datastructures/lists/arrays/FragmentedArrayList.h>
#include <core.h>

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

        IME_ASSERT(list.calcCount() == 6)
        IME_ASSERT(list.getCount() == 6)

        list.remove(index5);
        list.remove(index4);

        IME_ASSERT(list.getCount() == 6)
        IME_ASSERT(list.calcCount() == 4)

        list.remove(index6);

        IME_ASSERT(list.getCount() == 3)
        IME_ASSERT(list.calcCount() == 3)
    }

    int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {

        fragmentedArrayListTest();

    }




}