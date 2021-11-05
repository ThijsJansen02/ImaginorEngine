#pragma once
#include <core.h>
#include "../../iterators/iterator_C.h"
#include "../../View.h"
#include "allocation.h"

namespace IME::Data {

		template <typename T, byte *(*allocator)(sizeptr size), void (*deallocator)(sizeptr size, byte * data)>
		class DynamicArray_
		{
		public:
			using ReferenceType = T &;
			using PointerType = T *;
			using ValueType = T;

			using iterator = iterator_C_base<T>;
			using const_iterator = iterator_C_base_const<T>;

			/*
			DynamicArray(std::initializer_list<T> in)
			{

				m_Data = (ValueType *)allocator(in.size() * sizeof(ValueType));
				int index = 0;
				for (const T &it : in)
				{
					m_Data[index] = it;
				}

				m_Capacity = in.size();
			} */

			template<byte *(*allocator2)(sizeptr size), void (*deallocator2)(sizeptr size, byte * data)>
			DynamicArray_(const DynamicArray_<T, allocator2, deallocator2> &other)
			{
				m_Capacity = other.getCapacity();
				m_Data = (ValueType *)allocator(capacity * sizeof(ValueType));
				IME::Data::copy(begin(), end(), other.begin());
			}

			template<byte *(*allocator2)(sizeptr size), void (*deallocator2)(sizeptr size, byte * data)>
			DynamicArray_(DynamicArray_<T, allocator2, deallocator2> &&other) noexcept
			{
				this->m_Data = other.m_Data;
				this->m_Capacity = other.m_Capacity;

				other.m_Data = nullptr;
			}

			~DynamicArray_()
			{
				if(getCapacity() > 0) {
					deallocator(m_Capacity * sizeof(ValueType), (byte*)m_Data);
				}
			}

			DynamicArray_(uint32_t capacity)
			{
				m_Capacity = capacity;
				m_Data = (ValueType *)allocator(capacity * sizeof(ValueType));
			}
			DynamicArray_() = default;

			const ReferenceType operator[](uint32_t index) const
			{
				IME_DEBUG_ASSERT_BREAK(index < m_Capacity, "index to high")

				return m_Data[index];
			}

			ReferenceType operator[](uint32_t index)
			{
				IME_DEBUG_ASSERT_BREAK(index < m_Capacity, "index to high")

				return m_Data[index];
			}

			bool contains(T value) const
			{

				for (const T &it : m_Data)
				{
					if (it == value)
					{
						return true;
					}
				}
				return false;
			}

			inline View<iterator> view(uint32_t begin, uint32_t end)
			{
				IME_DEBUG_ASSERT_BREAK(begin < m_Capacity && end < m_Capacity, "stackoverflow")

				return View(iterator(&m_Data[begin]), iterator(&m_Data[end + 1]));
			}

			void 
			resize(uint32 newcapacity)
			{
				T *newarray = (ValueType *)allocator(newcapacity * sizeof(ValueType));

				uint32 oldcapacity = m_Capacity;
				(newcapacity < m_Capacity) ? m_Capacity = newcapacity : m_Capacity;

				for (int i = 0; i < m_Capacity; i++)
				{
					newarray[i] = m_Data[i];
				}

				if (oldcapacity > 0)
				{
					deallocator(oldcapacity * sizeof(ValueType), (byte*)m_Data);
				}

				m_Data = newarray;
				m_Capacity = newcapacity;
			}

			uint32_t getCapacity() const { return m_Capacity; }
			uint32_t getSize() const { return m_Capacity * sizeof(T); }

			iterator begin() { return iterator(m_Data); }
			iterator end() { return iterator(m_Data + m_Capacity); }

			const_iterator begin() const { return const_iterator(m_Data); }
			const_iterator end() const { return const_iterator(m_Data + m_Capacity); }

		protected:
			PointerType m_Data = nullptr;
			uint32_t m_Capacity = 0;
		};

		template<typename T>
		using DynamicArray =  DynamicArray_<T, std_allocator, std_deallocator>;
	}