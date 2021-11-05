#pragma once
#include "iterators/iterator_interface.h"

namespace IME::Data {

	template<typename T>
	class polymorphic_iterator {
	public:
		using ValueType = T;

		~polymorphic_iterator() {

			if (!m_IsReference) {
				delete it;
			}
		}

		polymorphic_iterator(iterator_interface<T>* in) {

			it = in;
		}

		polymorphic_iterator(polymorphic_iterator<T>&& cpy) noexcept {

			this->it = cpy.it;
			this->m_IsReference = cpy.m_IsReference;
			cpy.m_IsReference = true;
		}

		polymorphic_iterator(const polymorphic_iterator<T>& cpy) {

			this->it = cpy.it->copy();
			m_IsReference = false;
		}

		void operator=(const polymorphic_iterator<T>& cpy) {

			*this->it = cpy.it;
		}

		polymorphic_iterator createReference() {

			polymorphic_iterator result(it);
			result.m_IsReference = true;
			return result;
		}

		polymorphic_iterator<T> operator+(uint32_t add) const {

			polymorphic_iterator<T> newit(it->copy());
			newit += add;
			return newit;
		}

		polymorphic_iterator<T> operator-(uint32_t add) const {

			polymorphic_iterator<T> newit(it->copy());
			newit -= add;
			return newit;
		}

		void operator+=(uint32_t add) {

			(*it) += add;
		}

		void operator-=(uint32_t sub) {

			(*it) -= sub;
		}

		polymorphic_iterator operator++(int) {

			iterator_interface<T>* cpy = it->copy();
			(*it) += 1;
			return polymorphic_iterator(cpy);
		}

		polymorphic_iterator operator++() {
			
			(*it) += 1;
			return createReference();
		}

		polymorphic_iterator operator--(int) {

			iterator_interface<T>* cpy = it->copy();
			(*it) -= 1;
			return polymorphic_iterator(cpy);
		}

		polymorphic_iterator operator--() {

			(*it) -= 1;
			return createReference();
		}


		T& operator*() {
			return *(*it);
		}

		iterator_interface<T>* get() {
			return it;
		}

		inline bool operator==(const polymorphic_iterator& other) const {
			return *it == *other.it;
		}

		inline bool operator!=(const polymorphic_iterator& other) const {
			return (*it) != (other.it);
		}

		bool m_IsReference = false;
		iterator_interface<T>* it;
	};

	template<typename T>
	class polymorphic_iterator_const {
	public:
		using ValueType = T;

		~polymorphic_iterator_const() {

			if (!m_IsReference) {
				delete it;
			}
		}

		polymorphic_iterator_const(iterator_interface_const<T>* in) {

			it = in;
		}

		polymorphic_iterator_const(polymorphic_iterator_const<T>&& cpy) noexcept {

			this->it = cpy.it;
			this->m_IsReference = cpy.m_IsReference;
			cpy.m_IsReference = true;
		}

		polymorphic_iterator_const(const polymorphic_iterator_const<T>& cpy) {

			std::cout << "copied" << std::endl;
			this->it = cpy.it->copy();
		}

		void operator=(const polymorphic_iterator_const<T>& cpy) {
			std::cout << "copied" << std::endl;
			this->it = cpy.it->copy();
		}

		void operator=(polymorphic_iterator_const<T>&& cpy) {

			this->it = cpy.it;
			this->m_IsReference = cpy.m_IsReference;
			cpy.m_IsReference = true;

		}

		void operator+=(uint32_t add) {

			(*it) += add;
		}

		void operator-=(uint32_t sub) {

			(*it) -= sub;
		}

		polymorphic_iterator_const createReference() {

			polymorphic_iterator_const result(it);
			result.m_IsReference = true;

			return result;
		}


		polymorphic_iterator_const operator++(int) {

			iterator_interface_const<T>* cpy = it->copy();
			(*it) += 1;
			return polymorphic_iterator_const(cpy);
		}

		polymorphic_iterator_const operator++() {

			(*it) += 1;
			return createReference();
		}

		polymorphic_iterator_const operator--(int) {

			iterator_interface_const<T>* cpy = it->copy();
			(*it) -= 1;
			return polymorphic_iterator_const<T>(cpy);
		}

		polymorphic_iterator_const operator--() {

			(*it) -= 1;
			return createReference();
		}

		const T* operator->() const {
			return &(*(*it));
		}

		const T& operator*() const {
			return *(*it);
		}

		iterator_interface_const<T>* get() {
			return it;
		}

		inline bool operator==(const polymorphic_iterator_const& other) const {
			return *it == *other.it;
		}

		inline bool operator!=(const polymorphic_iterator_const& other) const {
			return (*it) != (other.it);
		}

		bool m_IsReference = false;
		iterator_interface_const<T>* it;
	};


}