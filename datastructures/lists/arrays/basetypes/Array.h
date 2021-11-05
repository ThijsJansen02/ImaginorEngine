#pragma once
#include <core.h>

#include "../../iterators/iterator_C.h"
#include "../../View.h"

namespace IME::Data {

	template<typename T, unsigned int count>
	class Array {
	public:
		using ValueType = T;
		using ReferenceType = T&;
		using PointerType = T*;

		using iterator = iterator_C_base<T>;
		using const_iterator = iterator_C_base_const<T>;

		Array() = default;
/*
		Array(const std::initializer_list<T>& in) {

			std::initializer_list<T>::iterator it = in.begin();
			for (int i = 0; i < count; i++) {
				m_Data[i] = it[i];
			}
		}
*/
		ValueType& operator[](uint32_t index);
		const ValueType& operator[](uint32_t index) const;


		Array(const Array& cpy);

		View<iterator> view(uint32_t begin, uint32_t end) { return View(this->begin() + begin, this->begin() + end + 1); }

		iterator begin() {
			return iterator(m_Data);
		}

		iterator end() {
			return iterator(m_Data + count);
		}

		const_iterator begin() const {
			return const_iterator(m_Data);
		}

		const_iterator end() const {
			return const_iterator(m_Data + count);
		}

		inline uint32_t getCapacity() const {
			return count;
		}

		inline uint32_t getSize() const {
			return count * sizeof(T);
		}

	private:
		T m_Data[count] = {};
	};

	template<typename T, unsigned int size>
	inline const T& Array<T, size>::operator[](uint32 index) const
	{
		return m_Data[index];
	}
	
	template<typename T, unsigned int size>
	inline T& Array<T, size>::operator[](uint32 index)
	{
		return m_Data[index];
	}

	template<typename T, unsigned int count>
	inline Array<T, count>::Array(const Array<T, count>& cpy)
	{
		for (uint32_t i = 0; i < count; i++) {
			m_Data[i] = cpy[i];
		}
	}


}