#pragma once
#include "../iterators/iterator_C.h"
#include "../View.h"
#include "basetypes/DynamicArray.h"

namespace IME::Data {

	template<typename T, byte*(*allocator)(sizeptr size), void(*deallocator)(sizeptr size, byte* data)>
	class ArrayList : public DynamicArray_<T, allocator, deallocator> {
	public:
		using ReferenceType = T&;
		using PointerType = T*;
		using ValueType = T;

		using iterator = iterator_C_base<T>;
		using const_iterator = iterator_C_base_const<T>;

		ArrayList() : DynamicArray_() {
			m_End = DynamicArray_::begin();
		}

		/*
		ArrayList(const std::initializer_list<T>& in) : DynamicArray(in) {
			m_End = DynamicArray::end();
		} */

		void resize(uint32_t newcapacity) {
			uint32_t oldcount = getCount();
			DynamicArray_::resize(newcapacity);
			
			m_End = begin() + oldcount;
		}

		inline void insert(const T& value, uint32_t index) {

			push_back(value);

			IM::Data::copy_reverse(begin() + index + 1, m_End, m_End - 1);
			(*this)[index] = value;
		}

		inline void clear() {

			m_End = begin();
		}

		View<iterator> view(uint32_t begin, uint32_t end) {
			return View<iterator>(this->begin() + begin, this->begin() + end + 1);
		}

		inline void shrink() {

			if (getCount() == getCapacity()) {
				return;
			}
			resize(getCount());
		}

		inline void push_back(const T& val) {
			if (getCount() >= DynamicArray_::getCapacity()) {
				resize((getCapacity() + 2) * 1.5);
			}

			m_Data[getCount()] = val;
			m_End++;
		}

		inline const ReferenceType pop() {
			pop_back();
		}

		inline void push(const T& value) {
			push_back(value);
		}

		const ReferenceType pop_back() {
			return *(--m_End);
		}


		ValueType pop_front() {
			T result = m_Data[0];

			for (int i = 1; i < getCount(); i++) {
				m_Data[i - 1] = m_Data[i];
			}

			m_End--;
			return result;
		}

		void push_front(const T& val) {
			if (getCount() >= DynamicArray_::getCapacity()) {
				uint32_t newcapacity = (getCapacity() + 1) * 1.5;
				uint32_t oldcount = getCount();

				ValueType* newarray = allocator(newcapacity * sizeof(ValueType));

				//(newcapacity < m_Capacity) ? m_Capacity = newcapacity : m_Capacity;

				for (int i = 0; i < m_Capacity; i++) {
					newarray[i + 1] = m_Data[i];
				}

				deallocator(m_Capacity * sizeof(ValueType), (byte*)m_Data);

				m_Data = newarray;
				m_Capacity = newcapacity;

				m_End = begin() + oldcount + 1;
			}
			else {
				for (int i = getCount() - 1; i >= 0; i--) {
					m_Data[i + 1] = m_Data[i];
				}
				m_End++;
			}

			m_Data[0] = val;
		}

		const ReferenceType operator[](uint32_t index) const {
			return m_Data[index];
		}

		ReferenceType operator[](uint32_t index) {
			return m_Data[index];
		}

		bool isEmpty() const {

			return begin() == end();

		}

		inline uint32_t getCount() {
			return begin().diff(end());
		}

		inline iterator end() {
			return m_End;
		};
		inline iterator begin() {
			return DynamicArray_::begin();
		}

		inline const_iterator end() const {
			return m_End;
		};
		inline const_iterator begin() const {
			return DynamicArray_::begin();
		}

	protected:
		iterator m_End;
	};
}