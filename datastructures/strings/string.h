#pragma once
#include <core.h>
#include <stdio.h>
#include <intrinsics.h>

namespace IME::Data {

    template <byte*(*alloc)(sizeptr size), void(*dealloc)(sizeptr size, byte* data)>
    class String
    {
    public:
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