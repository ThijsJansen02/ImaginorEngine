#pragma once
#include <core.h>
#include "basetypes/DynamicArray.h"
#include "../copying.h"
#include "../iterators/iterator_interface.h"

namespace IME::Data {

	template<typename T>
	class Circular_Iterator_D  : public iterator_interface<T> {
	public:
		Circular_Iterator_D(uint32_t offset, uint32_t capacity, iterator_C_base<T> membase) : count(capacity), m_Offset(offset), m_Membase(membase) {}

		using ValueType = T;
		using PointerType = ValueType*;
		using ReferenceType = ValueType&;

		inline void operator+=(uint32_t add) {
			m_Offset = (m_Offset + add) % count;
		}

		inline void operator-=(uint32_t sub) {
			for (int i = 0; i < sub; i++) {
				(*this)--;
			}
		}

		inline Circular_Iterator_D& operator++() {
			m_Offset = (++m_Offset) % count;
			return *this;
		}

		inline Circular_Iterator_D operator++(int) {
			Circular_Iterator_D it = *this;
			m_Offset = (++m_Offset) % count;
			return it;
		}

		inline Circular_Iterator_D& operator--() {

			m_Offset == 0 ? (m_Offset += count - 1) : (m_Offset--);
			return *this;
		}

		inline Circular_Iterator_D operator--(int) {
			Circular_Iterator_D it = *this;
			m_Offset == 0 ? (m_Offset += count - 1) : (m_Offset--);
			return it;
		}

		inline ValueType& operator[](uint32_t index) const {
			return *(*this + index);
		}

		inline PointerType operator->() const {
			return &(*(m_Membase + m_Offset));
		}


		inline ValueType& operator*()  const {
			return *(m_Membase + m_Offset);
		}

		inline bool operator==(const Circular_Iterator_D<T>& other) const {
			return this->m_Membase + this->m_Offset == other.m_Membase + other.m_Offset;
		}

		inline bool operator!=(const Circular_Iterator_D<T>& other) const {
			return !(*this == other);
		}

		inline bool operator==(iterator_interface* other) const override {

			Circular_Iterator_D<T>* otherc = static_cast<Circular_Iterator_D<T>*>(other);

			if (otherc != nullptr) {
				return *this == *otherc;
			}
			return false;
		}

		inline bool operator!=(iterator_interface* other) const override {
			Circular_Iterator_D<T>* otherc = static_cast<Circular_Iterator_D<T>*>(other);

			if (otherc != nullptr) {
				return *this != *otherc;
			}
			return false;

		}

		inline void operator=(iterator_interface<T>* other) override {

			Circular_Iterator_D<T>* otherc = static_cast<Circular_Iterator_D<T>*>(other);

			*this = *otherc;
		}

		virtual iterator_interface* copy() {
			return new Circular_Iterator_D<T>(*this);
		}

		Circular_Iterator_D operator+(uint32_t add) const;
		Circular_Iterator_D operator-(uint32_t sub) const;

		Circular_Iterator_D operator+(const Circular_Iterator_D<T>& itl) const;
		Circular_Iterator_D operator-(const Circular_Iterator_D<T>& itl) const;


		uint32_t diff(const Circular_Iterator_D& end) const {

			int32_t diff = end.m_Offset - this->m_Offset;
			return (diff >= 0) ? diff : diff + count;
		}

	private:
		template<typename T>
		friend class CircularArrayList;

		template<typename T>
		friend class Circular_Iterator_D_Const;

		Circular_Iterator_D() : count(0), m_Offset(0), m_Membase(nullptr) {}

		uint32_t count;
		uint32_t m_Offset;
		iterator_C_base<T> m_Membase;

	};

	template<typename T>
	inline Circular_Iterator_D<T> Circular_Iterator_D<T>::operator+(uint32_t add) const
	{
		return Circular_Iterator_D<T>((this->m_Offset + add) % count, count, this->m_Membase);
	}

	template<typename T>
	inline Circular_Iterator_D<T> Circular_Iterator_D<T>::operator-(uint32_t sub) const
	{
		Circular_Iterator_D it = *this;
		for (int i = 0; i < sub; i++) {
			it--;
		}
		return it;
	}

	template<typename T>
	inline Circular_Iterator_D<T> Circular_Iterator_D<T>::operator+(const Circular_Iterator_D<T>& itl) const
	{
		return *this + itl.m_Offset;
	}

	template<typename T>
	inline Circular_Iterator_D<T> Circular_Iterator_D<T>::operator-(const Circular_Iterator_D<T>& itl) const
	{
		return *this - itl.m_Offset;
	}

	template<typename T>
	class Circular_Iterator_D_Const : public iterator_interface_const<T> {
	public:
		Circular_Iterator_D_Const(uint32_t offset, uint32_t capacity, iterator_C_base<T> membase) : count(capacity), m_Offset(offset), m_Membase(membase) {}
		Circular_Iterator_D_Const(const Circular_Iterator_D<T>& other) : count(other.count), m_Offset(other.m_Offset), m_Membase(other.m_Membase) { }

		using ValueType = T;
		using PointerType = ValueType*;
		using ReferenceType = ValueType&;

		inline void operator+=(uint32_t add) override {
			m_Offset = (m_Offset + add) % count;
		}

		inline void operator-=(uint32_t sub) override {
			for (int i = 0; i < sub; i++) {
				(*this)--;
			}
		}

		inline Circular_Iterator_D_Const& operator++() {
			m_Offset = (++m_Offset) % count;
			return *this;
		}

		inline Circular_Iterator_D_Const operator++(int) {
			Circular_Iterator_D_Const it = *this;
			m_Offset = (++m_Offset) % count;
			return it;
		}

		inline Circular_Iterator_D_Const& operator--() {

			m_Offset == 0 ? (m_Offset += count - 1) : (m_Offset--);
			return *this;
		}

		inline Circular_Iterator_D_Const operator--(int) {
			Circular_Iterator_D_Const it = *this;
			m_Offset == 0 ? (m_Offset += count - 1) : (m_Offset--);
			return it;
		}

		inline const ValueType& operator[](uint32_t index) const override {
			return *(*this + index);
		}


		inline const PointerType operator->() const override {
			return &(*(m_Membase + m_Offset));
		}

		inline const ValueType& operator*() const override {
			return *(m_Membase + m_Offset);
		}

		inline bool operator==(const Circular_Iterator_D_Const<T>& other) const {
			return this->m_Membase + this->m_Offset == other.m_Membase + other.m_Offset;
		}

		inline bool operator!=(const Circular_Iterator_D_Const<T>& other) const {
			return !(*this == other);
		}

		inline void operator=(iterator_interface_const<T>* other) override {

			Circular_Iterator_D_Const<T>* otherc = static_cast<Circular_Iterator_D_Const<T>*>(other);

			*this = *otherc;
		}

		inline bool operator==(iterator_interface_const* other) const override {

			Circular_Iterator_D_Const<T>* otherc = static_cast<Circular_Iterator_D_Const<T>*>(other);

			if (otherc != nullptr) {
				return *this == *otherc;
			}
			return false;
		}

		inline bool operator!=(iterator_interface_const* other) const override {
			Circular_Iterator_D_Const<T>* otherc = static_cast<Circular_Iterator_D_Const<T>*>(other);

			if (otherc != nullptr) {
				return *this != *otherc;
			}
			return false;

		}

		virtual iterator_interface_const* copy() override {

			return new Circular_Iterator_D_Const<T>(*this);
		}

		Circular_Iterator_D_Const operator+(uint32_t add) const;
		Circular_Iterator_D_Const operator-(uint32_t sub) const;

		Circular_Iterator_D_Const operator+(const Circular_Iterator_D_Const<T>& itl) const;
		Circular_Iterator_D_Const operator-(const Circular_Iterator_D_Const<T>& itl) const;


		uint32_t diff(const Circular_Iterator_D_Const& end) const {

			int32_t diff = end.m_Offset - this->m_Offset;
			return (diff >= 0) ? diff : diff + count;
		}

	private:
		template<typename T>
		friend class CircularArrayList;

		Circular_Iterator_D_Const() : count(0), m_Offset(0), m_Membase(nullptr) {}

		uint32_t count;
		uint32_t m_Offset;
		iterator_C_base<T> m_Membase;
	};

	template<typename T>
	inline Circular_Iterator_D_Const<T> Circular_Iterator_D_Const<T>::operator+(uint32_t add) const
	{
		return Circular_Iterator_D_Const<T>((this->m_Offset + add) % count, count, this->m_Membase);
	}

	template<typename T>
	inline Circular_Iterator_D_Const<T> Circular_Iterator_D_Const<T>::operator-(uint32_t sub) const
	{
		Circular_Iterator_D_Const it = *this;
		for (int i = 0; i < sub; i++) {
			it--;
		}
		return it;
	}

	template<typename T>
	inline Circular_Iterator_D_Const<T> Circular_Iterator_D_Const<T>::operator+(const Circular_Iterator_D_Const<T>& itl) const
	{
		return *this + itl.m_Offset;
	}

	template<typename T>
	inline Circular_Iterator_D_Const<T> Circular_Iterator_D_Const<T>::operator-(const Circular_Iterator_D_Const<T>& itl) const
	{
		return *this - itl.m_Offset;
	}


	template<typename T, byte*(*allocator)(uint32 size), void(*deallocator)(uint32 size, byte* data)>
	class CircularArrayList : public IME::Data::DynamicArray<T, allocator, deallocator> {
	public:
		using ValueType = T;
		using ReferenceType = T&;
		using PointerType = T*;

		using iterator = Circular_Iterator_D<T>;
		using const_iterator = Circular_Iterator_D_Const<T>;

		CircularArrayList() : DynamicArray() {
			
			m_Capacity = 1;
		
		};
		CircularArrayList(const std::initializer_list<T>& in) : DynamicArray() {
			
			DynamicArray::resize(in.size() + 1);

			m_Begin = iterator(0, m_Capacity, DynamicArray::begin());
			m_End = iterator(getCapacity(), m_Capacity, DynamicArray::begin());

			IM::Data::copy(m_Begin, m_End, in.begin());
		}

		inline View<iterator> view(uint32_t begin, uint32_t end) {
			return View<iterator>(this->begin() + begin, this->begin() + end + 1);
		}

		inline void insert(const T& value, uint32_t index) {

			if (index < (getCount() / 2)) {
				push_front(value);
				IM::Data::copy(m_Begin, m_Begin + index, m_Begin + 1);
				(*this)[index] = value;
			}
			else {
				push_back(value);
				IM::Data::copy_reverse(m_Begin + index + 1, m_End, m_End - 1);
				(*this)[index] = value;

			}
		}

		inline void push(const T& val) {
			push_back(val);
		}

		inline const ReferenceType pop() {
			return pop_back();
		}

		inline void push_front(const T& val) {

			if (getCapacity() <= getCount()) {
				//std::cout << "resized!: " << getCapacity() << ", " << getCount() << std::endl;
				resize(getCapacity() * 1.5);
			}

			m_Begin--;
			*m_Begin = val;
		}

		inline void push_back(const T& val) {
			if (getCapacity() <= getCount()) {
				//std::cout << "resized!: " << getCapacity() << ", " << getCount() << std::endl;
				resize(m_Capacity * 1.5);
			}
			*m_End = val;
			m_End++;
		}

		inline const ReferenceType pop_front() {
			return *(m_Begin++);
		}

		inline const ReferenceType pop_back() {
			
			 return *(--m_End);

		}

		inline const ReferenceType operator[](uint32_t index) const {
			return m_Begin[index];
		}

		inline ReferenceType operator[](uint32_t index) {
			return m_Begin[index];
		}

		inline void clear() {

			m_Begin = iterator(0, m_Capacity, DynamicArray::begin());
			m_End = iterator(0, m_Capacity, DynamicArray::begin());

		}

		inline void shrink() {

			if (getCount() == getCapacity()) {
				return;
			}
			resize(getCount());
		}

		inline void resize(uint32 newcapacity) {
			newcapacity++;
			T* newarray = (ValueType*)allocator(newcapacity * sizeof(ValueType));

			(newcapacity < m_Capacity) ? m_Capacity = newcapacity : m_Capacity;

			IM::Data::copy(iterator_C_base<T>(newarray), iterator_C_base<T>(newarray + getCapacity()), m_Begin);

			m_Begin = iterator(0, newcapacity, iterator_C_base<T>(newarray));
			m_End = iterator(getCapacity(), newcapacity, iterator_C_base<T>(newarray));

			deallocator(m_Capacity * sizeof(ValueType), m_Data);
			m_Capacity = newcapacity;

			m_Data = newarray;

		}

		bool isEmpty() const {
			return begin() == end();
		}
 
		inline uint32_t getCount() const {
			return m_Begin.diff(m_End);
		}

		inline uint32_t getCapacity() const {
			return m_Capacity - 1;
		}

		inline const_iterator begin() const {
			return m_Begin;
		}

		inline const_iterator end() const {
			return m_End;
		}

		inline iterator begin() {
			return m_Begin;
		}

		inline iterator end() {
			return m_End;
		}

	private:

		iterator m_End;
		iterator m_Begin;
	};

	template<typename T>
	using CLArrayList = CircularArrayList<T>;

	template<typename T> 
	using vector = CircularArrayList<T>;



}