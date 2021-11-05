#pragma once
#include "iterator_interface.h"

namespace IME::Data {

	template<typename Node>
	class iterator_L_base : public iterator_interface<Node::template ValueType> {
	public:
		using ValueType = Node::template ValueType;
		using PointerType = ValueType*;
		using ReferenceType = ValueType&;

		iterator_L_base() : m_Ptr(nullptr) {}
		iterator_L_base(Node* node) : m_Ptr(node) {}

		inline void operator+=(uint32_t add) override{
			for (int i = 0; i < add; i++) {
				m_Ptr = m_Ptr->getNext();
			}
		}

		inline void operator-=(uint32_t sub) override {
			for (int i = 0; i < sub; i++) {
				m_Ptr = m_Ptr->getPrev();
			}
		}

		inline iterator_L_base& operator++() {
			m_Ptr = m_Ptr->getNext();
			return *this;
		}

		inline iterator_L_base operator++(int) {
			iterator_L_base it = *this;
			m_Ptr = m_Ptr->getNext();
			return it;
		}

		inline iterator_L_base& operator--() {
			m_Ptr = m_Ptr->getPrev();
			return *this;
		}

		inline iterator_L_base operator--(int) {
			iterator_L_base it = *this;
			m_Ptr = m_Ptr->getPrev();
			return it;
		}

		inline Node* getNode() {
			return m_Ptr;
		}

		inline ValueType& operator[](uint32_t index) const override {
			Node* result = m_Ptr;
			for (int i = 0; i < index; i++) {
				result = result->getNext();
			}

			return result->getValue();
		}

		inline PointerType operator->() const override{
			return &m_Ptr->getValue();
		}


		inline ValueType& operator*() const override {
			return m_Ptr->getValue();
		}


		inline bool operator==(const iterator_L_base<Node>& other) const {
			return (this->m_Ptr == other.m_Ptr);
		}

		inline bool operator!=(const iterator_L_base<Node>& other) const {
			return !(*this == other);
		}

		inline bool operator==(iterator_interface* other) const override {
			iterator_L_base<Node>* otherc = static_cast<iterator_L_base<Node>*>(other);

			if (otherc != nullptr) {
				return *this != *otherc;
			}
			return false;
		}

		inline bool operator!=(iterator_interface* other) const override {
			iterator_L_base<Node>* otherc = static_cast<iterator_L_base<Node>*>(other);

			if (otherc != nullptr) {
				return *this != *otherc;
			}
			return false;
		}


		iterator_L_base<Node> operator+(uint32_t add) const;
		iterator_L_base<Node> operator-(uint32_t sub) const;

		virtual iterator_interface* copy() {

			return new iterator_L_base(*this);

		}

	private:
		template<typename Node>
		friend class iterator_L_base_const;

		Node* m_Ptr;
	};


	template<typename Node>
	inline iterator_L_base<Node> iterator_L_base<Node>::operator+(uint32_t add) const
	{
		iterator_L_base<Node> result = *this;

		for (int i = 0; i < add; i++) {
			result.m_Ptr = result.m_Ptr->getNext();
		}

		return result;
	}

	template<typename Node>
	inline iterator_L_base<Node> iterator_L_base<Node>::operator-(uint32_t sub) const
	{
		iterator_L_base<Node> result = *this;

		for (int i = 0; i < add; i++) {
			result.m_Ptr = result.m_Ptr->getPrev();
		}

		return result;
	}



	template<typename Node>
	class iterator_L_base_const : public iterator_interface_const<Node::template ValueType> {
	public:
		using ValueType = Node::template ValueType;
		using PointerType = ValueType*;
		using ReferenceType = ValueType&;

		iterator_L_base_const() : m_Ptr(nullptr) {}
		iterator_L_base_const(Node* node) : m_Ptr(node) {}
		iterator_L_base_const(const iterator_L_base<Node>& copy) : m_Ptr(copy.m_Ptr) { }

		inline void operator+=(uint32_t add) override {
			for (int i = 0; i < add; i++) {
				m_Ptr = m_Ptr->getNext();
			}
		}

		inline void operator-=(uint32_t sub) override {
			for (int i = 0; i < sub; i++) {
				m_Ptr = m_Ptr->getPrev();
			}
		}

		inline iterator_L_base_const& operator++() {
			m_Ptr = m_Ptr->getNext();
			return *this;
		}

		inline iterator_L_base_const operator++(int) {
			iterator_L_base_const it = *this;
			m_Ptr = m_Ptr->getNext();
			return it;
		}

		inline iterator_L_base_const& operator--() {
			m_Ptr = m_Ptr->getPrev();
			return *this;
		}

		inline iterator_L_base_const operator--(int) {
			iterator_L_base_const it = *this;
			m_Ptr = m_Ptr->getPrev();
			return it;
		}

		inline Node* getNode() {
			return m_Ptr;
		}

		inline const ValueType& operator[](uint32_t index) const override {
			Node* result = m_Ptr;
			for (int i = 0; i < index; i++) {
				result = result->getNext();
			}

			return result->getValue();
		}

		inline const PointerType operator->() const override {
			return &m_Ptr->getValue();
		}

		inline const ValueType& operator*() const override {
			return m_Ptr->getValue();
		}

		inline bool operator==(const iterator_L_base_const<Node>& other) const {
			return (this->m_Ptr == other.m_Ptr);
		}

		inline bool operator!=(const iterator_L_base_const<Node>& other) const {
			return !(*this == other);
		}

		inline bool operator==(iterator_interface_const* other) const override {
			iterator_L_base_const<Node>* otherc = static_cast<iterator_L_base_const<Node>*>(other);

			if (otherc != nullptr) {
				return *this != *otherc;
			}
			return false;
		}

		inline bool operator!=(iterator_interface_const* other) const override {
			iterator_L_base_const<Node>* otherc = static_cast<iterator_L_base_const<Node>*>(other);

			if (otherc != nullptr) {
				return *this != *otherc;
			}
			return false;
		}


		iterator_L_base_const<Node> operator+(uint32_t add) const;
		iterator_L_base_const<Node> operator-(uint32_t sub) const;

		virtual iterator_interface_const* copy() {

			return new iterator_L_base_const(*this);

		}

	private:
		Node* m_Ptr;
	};


	template<typename Node>
	inline iterator_L_base_const<Node> iterator_L_base_const<Node>::operator+(uint32_t add) const
	{
		iterator_L_base_const<Node> result = *this;

		for (int i = 0; i < add; i++) {
			result.m_Ptr = result.m_Ptr->getNext();
		}

		return result;
	}

	template<typename Node>
	inline iterator_L_base_const<Node> iterator_L_base_const<Node>::operator-(uint32_t sub) const
	{
		iterator_L_base_const<Node> result = *this;

		for (int i = 0; i < add; i++) {
			result.m_Ptr = result.m_Ptr->getPrev();
		}

		return result;
	}

}