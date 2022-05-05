#pragma once
#include <core.h>
#include <stdio.h>
#include <intrinsics.h>

namespace IME::Data {

    template <byte*(*alloc)(sizeptr size), void(*dealloc)(sizeptr size, byte* data)>
    class String
    {
    public:
        String() = default;

        static String create(const char* str) {
            String result;
            result.set(str);
            return result;
        }

        static String createWrapper(char* str) {
            String result;
            result.m_Data = str;
            result.m_Size = 0;
            return result;
        }

        void set(const char* string) {
            if(m_Data) {
                clear();
            }

            sizeptr size = strlen(string) + 1;
            if(size > 1) {
                m_Size = size;
                m_Data = (char*)alloc(m_Size);
                IME::copy((byte*)string, (byte*)m_Data, m_Size);
            }
        }

        String copy() {
            String result;
            result.set(this->m_Data);
            return result;
        }

        bool operator==(String other) {
            return compareStrings(this->getCstring(), other.getCstring());
        }

        void insert(char c, uint32 pos) {

            IME_DEBUG_ASSERT_BREAK(m_Data, "string is not yet set");
            char* newstring = (char*)alloc(m_Size + 1);
            IME::copy((IME::byte*)m_Data, (IME::byte*)newstring, pos);
            *(newstring + pos) = c;
            IME::copy((IME::byte*)m_Data + pos, (IME::byte*)newstring + pos + 1, m_Size - pos);
            m_Size+=1;
            m_Data = newstring;
            m_Data[m_Size - 1] = 0;
        }

        char operator[](uint32 i) {
            return *(m_Data + i);
        }

        void remove(uint32 pos) {
            IME_DEBUG_ASSERT_BREAK(m_Data, "string is not yet set");
            char* newstring = (char*)alloc(m_Size-1);
            IME::copy((IME::byte*)m_Data, (IME::byte*)newstring, pos);
            IME::copy((IME::byte*)m_Data + pos + 1, (IME::byte*)newstring + pos, m_Size - pos - 1);
            m_Size-=1;
            m_Data = newstring;
            m_Data[m_Size - 1] = 0;
        }

        void remove(uint32 begin, uint32 end) {
            uint32 size_removed = end - begin;
            char* newstring = (char*)alloc(m_Size - size_removed);
            IME::copy((IME::byte*)m_Data, (IME::byte*)newstring, begin);
            IME::copy((IME::byte*)m_Data + end + 1, (IME::byte*)newstring + begin, m_Size - end - 1);
            m_Data = (char*)newstring;
            m_Size = m_Size - size_removed;
            m_Data[m_Size - 1] = 0;
        }
        

        void clear() {
            dealloc(m_Size, (byte*)m_Data);
            m_Size = 0;
            m_Data = nullptr;
        }
        char* getCstring() {
            return m_Data; 
        }

        const char* getCstring() const {
            return m_Data; 
        }
        sizeptr getLength() const {
            return m_Size - 1;
        }

        operator bool() {
            return m_Data != nullptr;
        }

    private:
        char* m_Data = nullptr;
        sizeptr m_Size = 0;
    };
}