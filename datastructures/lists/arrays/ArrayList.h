#pragma once
#include <core.h>
#include <intrinsics.h>

namespace IME::Data {

    typedef byte* allocator(sizeptr size);
    typedef void deallocator(sizeptr size, byte* data);

    template<typename T, allocator alloc, deallocator dealloc>
    class ArrayList_ {
    public:
        class iterator {
        public:
            iterator(T* ptr) : m_Ptr(ptr) {}
            
            T& operator*() {
                return *m_Ptr;
            }

            bool32 operator==(iterator other) {
                return this->m_Ptr == other.m_Ptr;
            }
             bool32 operator!=(iterator other) {
                return this->m_Ptr != other.m_Ptr;
            }
            void operator--() {
                m_Ptr--;
            }
            void operator++() {
                m_Ptr++;
            }
        private:
            T* m_Ptr;
        };

        inline static ArrayList_ create(sizeptr basecapacity) {
            
            ArrayList_ result;
            result.m_Data = nullptr;
            if(basecapacity > 0) {
                result.m_Data = (T*)alloc(basecapacity * sizeof(T));
            }
            result.m_Count = 0;
            result.m_Capacity = basecapacity;

            return result;
        }  

        inline static void destroy(ArrayList_& list) {
            dealloc(list.m_Capacity * sizeof(T), (byte*)list.m_Data);
            list.m_Capacity = 0;
            list.m_Count = 0;
            list.m_Data = nullptr;
        }

        inline static ArrayList_ copy(const ArrayList_& list) {
            ArrayList_ result;
            result.m_Data = (T*)alloc(list.capacity * sizeof(T));
            result.m_Capacity = list.capacity;
            result.m_Count = list.count;
            IME::copy(list.m_Data, result.m_Data, list.m_Capacity * sizeof(T));

            return result;
        }

        inline static ArrayList_ move(const ArrayList_& list) {
            ArrayList_ result = list;
            list = {};
            return result;
        }

        inline void push_back(const T& data) {
            if(m_Count + 1 > m_Capacity) {
                resize((sizeptr)((real32)(m_Capacity + 1) * 1.5f));
            }
            m_Data[m_Count++] = data;
        }

        inline void push_front(const T& data) {
            if(m_Count + 1 > m_Capacity) {
                sizeptr newcapacity = (m_Capacity + 1) * 1.5;

                T* newdata = (T*)alloc(newcapacity);
                IME::copy(data, newdata + 1, m_Count * sizeof(T));
                if(m_Data != nullptr) {
                    dealloc(m_Capacity * sizeof(T), (byte*)m_Data);
                }
                m_Data = newdata;
                m_Capacity = newcapacity;
                m_Data[0] = data;
                ++m_Count;
            } else {
                IME::copy(data, data + 1, m_Count * sizeof(T));
                m_Data[0] = data;
                ++m_Count;
            }
        }

        inline T pop_back() {
            --m_Count;
            IME_DEBUG_ASSERT_BREAK(m_Count >=0, "array already empty!")
            return m_Data[m_Count];
        }

        inline T pop_front() {
            --m_Count;
            IME_DEBUG_ASSERT_BREAK(m_Count >= 0, "array already empty!")
            T result = m_Data[0];
            IME::copy(m_Data + 1, m_Data, count * sizeof(T))
            return result;
        }

        inline void resize(sizeptr capacity) {
            IME_DEBUG_ASSERT_BREAK(capacity >= m_Count, "cant resize to a capacity smaller than the count!"); 

            T* newdata = (T*)alloc(capacity * sizeof(T));
            IME::copy((byte*)m_Data, (byte*)newdata, m_Count * sizeof(T));
            if(m_Data != nullptr) {
                dealloc(m_Capacity * sizeof(T), (byte*)m_Data);
            }
            m_Capacity = capacity;
            m_Data = newdata;
        }

        inline void removeInterval(sizeptr begin, sizeptr end) {
            sizeptr removedsize = end - begin;
            IME::copy((byte*)(m_Data + end), (byte*)(m_Data + begin), removedsize * sizeof(T));
            m_Count -= removedsize;
        }

        inline void shrink() {
            resize(m_Count);
        }

        inline sizeptr getCount() {
            return m_Count;
        }

        inline sizeptr getCapacity() {
            return m_Capacity;
        }

        T& operator[](sizeptr index) {
            return m_Data[index];
        }

        inline void clear() {
            m_Count = 0;
        }

        inline iterator begin() const {
            return {m_Data};
        }

        inline iterator end() const {
            return {m_Data + m_Count};
        }

    private:

        T* m_Data;
        sizeptr m_Count;
        sizeptr m_Capacity;

    };
}