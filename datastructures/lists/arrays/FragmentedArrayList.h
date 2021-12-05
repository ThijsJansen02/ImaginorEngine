#include <core.h>
#include <intrinsics.h>
#include <string.h>

namespace IME::Data {

    template<typename T, byte*(*allocator)(sizeptr size), void(*deallocator)(sizeptr size, byte* data)>
    class FragmentedArrayList {
    private:    
    public:
        struct DataChunk {
            T data;
            bool32 isoccupied;
        };

        static FragmentedArrayList createFragmentedArrayList(sizeptr basecapacity) {
            FragmentedArrayList result;
            result.m_Capacity = basecapacity;
            if (basecapacity > 0) {
                result.m_Data = (DataChunk*)allocator(basecapacity * sizeof(DataChunk));
            }
            else {
                result.m_Data = nullptr;
            }
            result.m_Firstopenspot = 0;
            result.m_Count = 0;
            return result;
        }
        
        void resize(sizeptr newcapacity) {
            IME_DEBUG_ASSERT_BREAK(newcapacity >= m_Count, "new capacity must be higher than count")

            DataChunk* newarray = (DataChunk*)allocator(newcapacity * sizeof(DataChunk));
            memset(newarray, 0, newcapacity * sizeof(DataChunk));
            IME::copy((byte*)m_Data, (byte*)newarray, m_Count * sizeof(DataChunk));
            deallocator(m_Capacity * sizeof(DataChunk), (byte*)m_Data);
            m_Capacity = newcapacity;
            m_Data = newarray;
        }

        void remove(sizeptr index) {
            IME_DEBUG_ASSERT_BREAK(m_Data[index].isoccupied && index < m_Count, "index cant be accessed!")

            m_Data[index].isoccupied = false;
            if(index == m_Count - 1) {
                m_Count--;
                while(m_Count > 0 && !m_Data[m_Count - 1].isoccupied) {
                    m_Count--;
                }
            }
            if(index < m_Firstopenspot) {
                m_Firstopenspot = index;
            }
        }

        sizeptr add(const T& element) {
            sizeptr result = m_Firstopenspot;
            m_Data[m_Firstopenspot++] = {element, true};
            m_Count++;
            while(true) {
                for(; m_Firstopenspot < m_Capacity; m_Firstopenspot++) {
                    if(m_Data[m_Firstopenspot].isoccupied == false) {
                        return result;
                    } 
                }
                resize((sizeptr)(((real64)(m_Capacity + 1) * 1.5)));
            }
        }

        DataChunk& operator[](sizeptr index) {
            IME_DEBUG_ASSERT_BREAK(m_Data[index].isoccupied && index < m_Count, "data is not occupied")
            return m_Data[index];
        }

        const DataChunk& operator[](sizeptr index) const {
            IME_DEBUG_ASSERT_BREAK(m_Data[index].isoccupied && index < m_Count, "data is not occupied")
            return m_Data[index];
        }

        DataChunk& getUnchecked(sizeptr index) {
            return m_Data[index];
        }

        const DataChunk& getUnchecked(sizeptr index) const {
            return m_Data[index];
        }

        sizeptr calcCount() {
            sizeptr result = 0;
            for(sizeptr i = 0; i < m_Count; i++) {
                if(m_Data[i].isoccupied) {
                    ++result;
                }
            }
            return result;
        }

        sizeptr getCount() const {
            return m_Count;
        }
        sizeptr getCapacity() const {
            return m_Capacity;
        }

        char* toString(char* buffer, sizeptr buffersize) {
            
            if(buffersize < 21) {
                buffer[0] = '\0';
                return buffer;
            }
            strcpy(buffer, "fragmented array: (");
            char* ptr = &buffer[19];
            for(uint32 i = 0; i < m_Count; i++) {
                if(ptr - buffer >= buffersize) {
                    buffer[buffersize - 1] = '\0';
                    return buffer;
                }
                if(m_Data[i].isoccupied) {
                    ptr[0] = '1';
                    ptr[1] = ',';
                    ptr[2] = ' ';
                } else {
                    ptr[0] = '0';
                    ptr[1] = ',';
                    ptr[2] = ' ';
                }
                ptr+=3;
            }
            ptr[0] = ')';
            ptr[1] = '\0';
            return buffer;
        }
    private:
        
        sizeptr m_Count;
        sizeptr m_Capacity;
        sizeptr m_Firstopenspot;
        DataChunk* m_Data;
    };

}