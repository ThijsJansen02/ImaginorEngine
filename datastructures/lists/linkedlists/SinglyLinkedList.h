#pragma once
#include <core.h>
#include "../View.h"
#include "../iterators/iterator_L.h"

namespace IME::Data {

	template<typename T, byte*(*allocator)(uint32 size), void(*deallocator)(uint32 size, byte* data)>
	class SinglyLinkedList {
	public:
		struct Node {
		public:
			using ValueType = T;

			Node(const T& val, Node* next) : m_Next(next), m_Val(val) {};

			Node* getNext() {
				return m_Next;
			}

			Node* getPrev() {
				return nullptr;
			}

			ValueType& getValue() {
				return m_Val;
			}

			const ValueType& getValue() const {
				return m_Val;
			}

		protected:
			friend class SinglyLinkedList<T>;

			Node* m_Next = nullptr;
			ValueType m_Val;
		};

		using ValueType = T;
		using ReferenceType = ValueType&;
		using PointerType = ValueType*;

		using iterator = iterator_L_base<Node>;
		using const_iterator = iterator_L_base_const<Node>;

		SinglyLinkedList() : m_Head(nullptr) {

		}

		SinglyLinkedList(std::initializer_list<T> in) : m_Head(nullptr) {

			std::initializer_list<T>::iterator begin_it = in.begin();
			std::initializer_list<T>::iterator end_it = in.end();

			while (begin_it != end_it) {
				push_front(*(--end_it));
			}

			//push_front(*end_it);

		}


		void push_back(const T& val) {
			m_Count++;
			Node* newnode = allocator(sizeof(Node));
			*newnode = Node(val, nullptr);

			iterator it(m_Head);

			while (true) {
				if (it.getNode()->getNext() == nullptr) {
					break;
				}
				m_Head++;
			}

			it.getNode()->m_Next = newnode;
		}

		void push_front(const T& val) {
			m_Count++;
			Node* newnode = allocator(sizeof(Node))
			*newnode = Node(val, m_Head);
			m_Head = newnode;
		}

		ValueType pop_front() {
			Node* result = m_Head;
			ValueType resultval = m_Head->getValue();
			m_Head = m_Head->getNext();
			deallocator(sizeof(node), result);
			m_Count--;

			return resultval;
		}

		ValueType pop_back() {
			iterator it(m_Head);

			Node* prev = nullptr;
			while (true) {

				if (it.getNode()->getNext() == nullptr) {
					break;
				}

				prev = it.getNode();
				it++;
			}
			prev->m_Next = nullptr;

			ValueType result = *it;
			deallocator(sizeof(Node), it.getNode());

			m_Count--;

			return result;
		}

		uint32_t getCount() const {
			return m_Count;
		}

		uint32_t getCapacity() const {
			return m_Count;
		}

		const T& operator[](uint32_t index) const {
			iterator it = m_Head;

			for (int i = 0; i < index; i++) {
				it++;
			}

			return *it;
		}

		T& operator[](uint32_t index) {
			iterator it = m_Head;

			for (int i = 0; i < index; i++) {
				it++;
			}

			return *it;
		}

		void insert(const T& val) {
			if (index > m_Count / 2) {


				iterator it = iterator(m_Tail);
				for (int i = m_Count; i >= index; i--) {
					it--;
				}
				
				Node* before = it.getNode();
				Node* after = ++it.getNode();

				Node* newnode = allocator(sizeof(Node));
				*newnode = Node(val, after);
				before->m_Next = newnode;
				
			}
			else {

				iterator it = iterator(m_Head);

				for (int i = 0; i < index - 1; i++) {
					it++;
				}

				Node* before = it.getNode();
				Node* after = ++it.getNode();

				Node* newnode = allocator(sizeof(Node));
				*newnode = Node(val, after);
				before->m_Next = newnode;
			}
			m_Count++;
		}

		iterator begin() {
			return iterator(m_Head);
		}

		iterator end() {
			return iterator(nullptr);
		}

		const_iterator begin() const {
			return const_iterator(m_Head);
		}

		const_iterator end() const {
			return const_iterator(nullptr);
		}

		bool isEmpty() const {
			begin() == end();
		}

		void push(const T& val) {
			push_front(val);
		}

		ValueType pop(const T& val) {
			return pop_front();
		}

		void clear() {
			
			iterator begin = this->begin();
			iterator end = this->end();
	
			while (begin != end) {
				deallocator(sizeof(Node), begin->getNode());
				begin++;
			}

			m_Head = nullptr;
		}

		View<iterator> view(uint32_t begin, uint32_t end) {
			iterator it_end;
			iterator it_begin;

			iterator it = m_Head; 
			for (int i = 0; i <= end; i++) {
				if (i == begin) {
					it_begin = it;
				}
				it++;
			}
			it_end = it;

			return View(it_begin, it_end);
		}
	private:

		uint32_t m_Count = 0;
		Node* m_Head = nullptr;

	};


}