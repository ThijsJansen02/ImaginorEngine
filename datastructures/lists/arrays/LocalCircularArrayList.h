#pragma once
#include "basetypes/Array.h"
#include "../iterators/iterator_C.h"
#include "../copying.h"
#include  "../View.h"
#include "../iterators/iterator_interface.h"

namespace IME::Data {

	/// <summary>
	/// iterator for circular array
	/// </summary>
	/// <typeparam name="T"></typeparam>
	template<typename T, int count>
	class Circular_Iterator : public iterator_interface<T> {
	public:
		Circular_Iterator(uint32_t offset, iterator_C_base<T> membase) : m_Offset(offset), m_Membase(membase) {}

		using ValueType = T;
		using PointerType = ValueType*;
		using ReferenceType = ValueType&;

		inline void operator+=(uint32_t add) {
			m_Offset = (m_Offset + add) % count ;
		}

		inline void operator-=(uint32_t sub) {
			*this = *this - sub;
		}

		inline Circular_Iterator& operator++() {
			m_Offset = (++m_Offset) % count;
			return *this;
		}

		inline Circular_Iterator operator++(int) {
			Circular_Iterator it = *this;
			m_Offset = (++m_Offset) % count;
			return it;
		}

		inline Circular_Iterator& operator--() {
	
			m_Offset == 0 ? (m_Offset += count - 1) : (m_Offset--);
			return *this;
		}

		inline Circular_Iterator operator--(int) {
			Circular_Iterator it = *this;
			m_Offset == 0 ? (m_Offset += count - 1) : (m_Offset--);
			return it;
		}

		inline ValueType& operator[](uint32_t index) const {
			return *(*this + index);
		}

		inline PointerType operator->() const {
			return &(*(m_Membase + m_Offset));
		}

		inline ValueType& operator*() const {
			return *(m_Membase + m_Offset);
		}

		inline bool operator==(const Circular_Iterator<T, count>& other) const {
			return this->m_Membase + this->m_Offset == other.m_Membase + other.m_Offset;
		}

		inline bool operator!=(const Circular_Iterator<T, count>& other) const {
			return !(*this == other);
		}

		Circular_Iterator operator+(uint32_t add) const;
		Circular_Iterator operator-(uint32_t sub) const;

		Circular_Iterator operator+(const Circular_Iterator<T, count>& itl) const;
		Circular_Iterator operator-(const Circular_Iterator<T, count>& itl) const;

		uint32_t diff(const Circular_Iterator<T, count>& end) const {

			int32_t diff = end.m_Offset - this->m_Offset;
			return (diff >= 0) ? diff : diff + count;
		}

	private:

		uint32_t m_Offset;
		iterator_C_base<T> m_Membase;

	};

	template<typename T, int count>
	inline Circular_Iterator<T, count> Circular_Iterator<T, count>::operator+(uint32_t add) const
	{
		return Circular_Iterator<T, count>((this->m_Offset + add) % count, this->m_Membase);
	}

	template<typename T, int count>
	inline Circular_Iterator<T, count> Circular_Iterator<T, count>::operator-(uint32_t sub) const
	{
		Circular_Iterator it = *this;
		for (int i = 0; i < sub; i++) {
			it--;
		}
		return it;
	}

	template<typename T, int count>
	inline Circular_Iterator<T, count> Circular_Iterator<T, count>::operator+(const Circular_Iterator<T, count>& itl) const
	{
		return *this + itl.m_Offset;
	}

	template<typename T, int count>
	inline Circular_Iterator<T, count> Circular_Iterator<T, count>::operator-(const Circular_Iterator<T, count>& itl) const
	{
		return *this - itl.m_Offset;
	}

	/// <summary>
	/// circular array
	/// 
	/// loops back around when pushing front
	/// </summary>
	/// <typeparam name="T"></typeparam>
	template<typename T, int capacity>
	class LocalCircularArrayList : public IME::Data::Array<T, capacity + 1> {
	public:
		using ValueType = T;
		using PointerType = ValueType*;
		using ReferenceType = ValueType&;

		using iterator = Circular_Iterator<T, capacity + 1>;

		LocalCircularArrayList() : m_Begin(0, Array::begin()), m_End(0, Array::begin()) {

		}

		/*
		LocalCircularArrayList(const std::initializer_list<T>& in) : Array(in), m_Begin(0, Array::begin()), m_End(0, Array::begin()) {
			m_End += in.size();
		}*/

		inline void insert(const T& value, uint32_t index) {

			push_front(value);
			
			IM::Data::copy(m_Begin, m_Begin + index, m_Begin + 1);
			(*this)[index] = value;
		}

		inline void push_back(const T& element) {
			*m_End = element;
			m_End++;
		}

		inline void push_front(const T& element) {
			m_Begin--;
			*m_Begin = element;
		}

		inline void push(const T& element) {
			push_back(element);
		}

		inline const ReferenceType pop() {
			return pop_back();
		}

		inline const ReferenceType pop_back() {
			return *(--m_End);
		}

		inline const ReferenceType pop_front() {
			return *(m_Begin++);
		}

		inline ReferenceType operator[](uint32_t index) {
			return m_Begin[index];
		}

		inline const ReferenceType operator[](uint32_t index) const { 
			return m_Begin[index];
		}

		inline uint32_t getCapacity() {
			return capacity;
		}

		inline uint32_t getCount() {
			return m_Begin.diff(m_End);
		}

		inline uint32_t getSize() {
			return getCount() * sizeof(T);
		}

		View<iterator> view(uint32_t begin, uint32_t end) { return View(m_Begin + begin, m_Begin + end + 1); }

		inline bool contains(ValueType value) {

			for (const ReferenceType it : *this) {
				if (value == it) {
					return true;
				}
			}

			return false;

		}


		inline iterator begin() { return m_Begin; };
		inline iterator end() { return m_End; }

	private:

		iterator m_Begin;
		iterator m_End;

	};

	template<typename T, int capacity>
	using LCLArrayList = LocalCircularArrayList<T, capacity>;

}