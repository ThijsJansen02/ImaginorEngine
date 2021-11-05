#pragma once
#include "iterator_interface.h"
#include <intrinsics.h>

namespace IME::Data {

	template<typename T>
	class iterator_C_base : public iterator_interface<T> {
	public:
		using ValueType = T;
		using PointerType = ValueType*;
		using ReferenceType = ValueType&;

		iterator_C_base() : m_ptr(nullptr) {};
		iterator_C_base(PointerType ptr) : m_ptr(ptr) {}

		inline void operator+=(uint32_t add) override {
			m_ptr += add;
		}

		inline void operator-=(uint32_t sub) override {
			m_ptr -= sub;
		}

		inline iterator_C_base& operator++() {
			m_ptr++;
			return *this;
		 }

		inline iterator_C_base operator++(int) {
			iterator_C_base it = *this;
			m_ptr++;
			return it;
		}

		inline iterator_C_base& operator--() {
			m_Ptr--;
			return *this;
		}

		inline iterator_C_base operator--(int) {
			iterator_C_base it = *this;
			m_ptr--;
			return it;
		}

		inline ValueType& operator[](uint32_t index) const override {
			return *(m_ptr + index);
		}

		inline PointerType operator->() const override {
			return m_ptr;
		}

		inline ValueType& operator*() const override {
			return *m_ptr;
		}

		inline bool operator<=(const iterator_C_base<T>& other) const {
			return this->m_ptr <= other.m_ptr;
		}

		inline bool operator>=(const iterator_C_base<T>& other) const {
			return this->m_ptr >= other.m_ptr;
		}

		inline bool operator<(const iterator_C_base<T>& other) const {
			return this->m_ptr < other.m_ptr;
		}

		inline bool operator>(const iterator_C_base<T>& other) const {
			return this->m_ptr > other.m_ptr;
		}

		iterator_C_base<T> operator+(uint32_t add) const;
		iterator_C_base<T> operator-(uint32_t sub) const;

		iterator_C_base<T> operator+(iterator_C_base<T> itl) const;
		iterator_C_base<T> operator-(iterator_C_base<T> itl) const;

		uint32_t diff(const iterator_C_base<T>& end) const {

			return IME::absolute(this->m_ptr - end.m_ptr);

		}

		inline bool operator==(const iterator_C_base<T>& other) const {
			return this->m_ptr == other.m_ptr;
		}

		inline bool operator!=(const iterator_C_base<T>& other) const {
			return !(*this == other);
		}

		inline bool operator==(iterator_interface<T>* other) const override {
			iterator_C_base<T>* otherc = static_cast<iterator_C_base<T>*>(other);

			if (otherc != nullptr) {
				return *this == *otherc;
			}
			return false;
		}

		inline bool operator!=(iterator_interface<T>* other) const override {
			iterator_C_base<T>* otherc = static_cast<iterator_C_base<T>*>(other);

			if (otherc != nullptr) {
				return *this != *otherc;
			}
			return false;
		}

		iterator_interface* copy() override {
			return new iterator_C_base<T>(*this);
		}

	private:
		template<typename T>
		friend class iterator_C_base_const;

		PointerType m_ptr;
	};


	template<typename T>
	inline iterator_C_base<T> iterator_C_base<T>::operator+(uint32_t add) const
	{
		return m_ptr + add;
	}

	template<typename T>
	inline iterator_C_base<T> iterator_C_base<T>::operator-(uint32_t sub) const
	{
		return m_ptr - sub;
	}

	template<typename T>
	inline iterator_C_base<T> iterator_C_base<T>::operator+( iterator_C_base<T> itl) const
	{
		return m_ptr + itl.m_ptr;
	}

	template<typename T>
	inline iterator_C_base<T> iterator_C_base<T>::operator-(iterator_C_base<T> itl) const
	{
		return m_ptr - itl.m_ptr;
	}


	template<typename T>
	class iterator_C_base_const : public iterator_interface_const<T> {
	public:
		using ValueType = T;
		using PointerType = ValueType*;
		using ReferenceType = ValueType&;

		iterator_C_base_const() : m_ptr(nullptr) {};
		iterator_C_base_const(PointerType ptr) : m_ptr(ptr) {}
		iterator_C_base_const(const iterator_C_base<T>& cpy) : m_ptr(cpy.m_ptr) {
		}

		inline void operator+=(uint32_t add) override {
			m_ptr += add;
		}

		inline void operator-=(uint32_t sub) override {
			m_ptr -= sub;
		}

		inline iterator_C_base_const& operator++() {
			m_ptr++;
			return *this;
		}

		inline iterator_C_base_const operator++(int) {
			iterator_C_base_const it = *this;
			m_ptr++;
			return it;
		}

		inline iterator_C_base_const& operator--() {
			m_Ptr--;
			return *this;
		}

		inline iterator_C_base_const operator--(int) {
			iterator_C_base_const it = *this;
			m_ptr--;
			return it;
		}

		inline const ValueType& operator[](uint32_t index) const override {
			return *(m_ptr + index);
		}

		inline const PointerType operator->() const override {
			return m_ptr;
		}

		inline const ValueType& operator*() const override {
			return *m_ptr;
		}

		inline bool operator<=(const iterator_C_base_const<T>& other) const {
			return this->m_ptr <= other.m_ptr;
		}

		inline bool operator>=(const iterator_C_base_const<T>& other) const {
			return this->m_ptr >= other.m_ptr;
		}

		inline bool operator<(const iterator_C_base_const<T>& other) const {
			return this->m_ptr < other.m_ptr;
		}

		inline bool operator>(const iterator_C_base_const<T>& other) const {
			return this->m_ptr > other.m_ptr;
		}

		iterator_C_base_const<T> operator+(uint32_t add) const;
		iterator_C_base_const<T> operator-(uint32_t sub) const;

		iterator_C_base_const<T> operator+(iterator_C_base_const<T> itl) const;
		iterator_C_base_const<T> operator-(iterator_C_base_const<T> itl) const;

		uint32_t diff(const iterator_C_base_const<T>& end) const {

			return IME::absolute(this->m_ptr - end.m_ptr);

		}

		inline bool operator==(const iterator_C_base_const<T>& other) const {
			return this->m_ptr == other.m_ptr;
		}

		inline bool operator!=(const iterator_C_base_const<T>& other) const {
			return !(*this == other);
		}

		inline bool operator==(iterator_interface_const<T>* other) const override {
			iterator_C_base_const<T>* otherc = static_cast<iterator_C_base_const<T>*>(other);

			if (otherc != nullptr) {
				return *this == *otherc;
			}
			return false;
		}

		inline bool operator!=(iterator_interface_const<T>* other) const override {
			iterator_C_base_const<T>* otherc = static_cast<iterator_C_base_const<T>*>(other);

			if (otherc != nullptr) {
				return *this != *otherc;
			}
			return false;
		}

		iterator_interface_const* copy() override {
			return new iterator_C_base_const<T>(*this);
		}

	private:
		PointerType m_ptr;
	};


	template<typename T>
	inline iterator_C_base_const<T> iterator_C_base_const<T>::operator+(uint32_t add) const
	{
		return m_ptr + add;
	}

	template<typename T>
	inline iterator_C_base_const<T> iterator_C_base_const<T>::operator-(uint32_t sub) const
	{
		return m_ptr - sub;
	}

	template<typename T>
	inline iterator_C_base_const<T> iterator_C_base_const<T>::operator+(iterator_C_base_const<T> itl) const
	{
		return m_ptr + itl.m_ptr;
	}

	template<typename T>
	inline iterator_C_base_const<T> iterator_C_base_const<T>::operator-(iterator_C_base_const<T> itl) const
	{
		return m_ptr - itl.m_ptr;
	}


}