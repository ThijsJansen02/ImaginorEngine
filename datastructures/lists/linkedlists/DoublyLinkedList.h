#pragma once
#include <core.h>
#include "../iterators/iterator_L.h"
#include "../View.h"

namespace IME::Data {

	template<typename T, byte*(*allocator)(sizeptr size), void(*deallocator)(sizeptr size, byte* data)>
	class DoublyLinkedList_ {
	public:
		struct Node {
		public:
			using ValueType = T;

			Node(const T& val, Node* next, Node* prev) : m_Next(next), m_Prev(prev), m_Val(val) {};

			Node* getNext() {
				return m_Next;
			}

			Node* getPrev() {
				return m_Prev;
			}

			ValueType& getValue() {
				return m_Val;
			}

			const ValueType& getValue() const {
				return m_Val;
			}
			friend class DoublyLinkedList_;

		protected:

			Node* m_Next = nullptr;
			Node* m_Prev = nullptr;
			ValueType m_Val;
		};

		struct ProxyNode {
			Node* m_Next = nullptr;
			Node* m_Prev = nullptr;
		};

		struct iterator {
			iterator(Node* node) {
				m_Node = node;
			}

			void operator++() {
				m_Node = m_Node->getNext();
			}

			void operator--() {
				m_Node = m_Node->getPrev();
			}

			bool operator==(iterator other) {
				return this->m_Node == other.m_Node;
			}

			bool operator!=(iterator other) {
				return !(this->m_Node == other.m_Node);
			}

			Node* operator*() {
				return m_Node;
			}

			Node* m_Node;
		};


		using ValueType = T;
		using ReferenceType = ValueType&;
		using PointerType = ValueType*;

		using iterator = iterator;
		using const_iterator = iterator_L_base_const<Node>;

		void init() {
			m_Count = 0;
			Node* tail = (Node*)allocator(sizeof(ProxyNode));
			tail->m_Next = tail;
			tail->m_Prev = tail;
			m_Start = tail;
		}

		iterator begin() { return iterator(m_Start->getNext()); }
		iterator end() { return iterator(m_Start); }

		const_iterator begin() const { return const_iterator(m_Start->getNext()); }
		const_iterator end() const { return const_iterator(m_Start); }

		View<iterator> view(uint32_t begin, uint32_t end) {

			iterator begin_it;
			iterator end_it;

			iterator it(m_Head);

			for (int i = 0; i <= end; i++) {

				if (i == begin) {
					begin_it = it;
				}
				it++;
			}

			end_it = it;

			return View(begin_it, end_it);
		}

		void clear() {

			iterator begin = this->begin();
			iterator end = this->end();

			while (begin != end) {
				deallocator(sizeof(Node), begin->getNode());
				begin++;
			}
		}

		void insert(const T& val, uint32_t index) {

			if (index > m_Count / 2) {

				//std::cout << "used upper it" << std::endl;
				iterator it = iterator(m_Start);
				for (int i = m_Count; i > index; i--) {
					it--;
				}
				Node* insert = it.getNode();
				Node* newnode = allocator(sizeof(Node));
				*newnode = Node(val, insert, insert->getPrev());

				insert->getPrev()->m_Next = newnode;
				insert->m_Prev = newnode;
			} else {

				iterator it = iterator(m_Head);

				for (int i = 0; i < index; i++) {
					it++;
				}

				Node* insert = it.getNode();
				Node* newnode = allocator(sizeof(Node)); 
				*newnode = Node(val, insert, insert->getPrev());

				insert->getPrev()->m_Next = newnode;
				insert->m_Prev = newnode;
			}
			m_Count++;
		}

		void remove(uint32_t index) {

			if (index > m_Count / 2) {

				//std::cout << "used upper it" << std::endl;
				iterator it = iterator(m_Tail);
				for (int i = m_Count; i > index; i--) {
					it--;
				}
				Node* insert = it.getNode();
				Node* newnode = allocator(sizeof(Node));
				*newnode = Node(val, insert, insert->getPrev());

				insert->getPrev()->m_Next = newnode;
				insert->m_Prev = newnode;
			} else {

				iterator it = iterator(m_Head);

				for (int i = 0; i < index; i++) {
					it++;
				}

				Node* insert = it.getNode();
				Node* newnode = allocator(sizeof(Node)); 
				*newnode = Node(val, insert, insert->getPrev());

				insert->getPrev()->m_Next = newnode;
				insert->m_Prev = newnode;
			}
			m_Count++;
		}

		void remove(Node* node) {
			Node* prev = node->m_Prev;
			Node* next = node->m_Next;
			
			prev->m_Next = next;
			next->m_Prev = prev;

			deallocator(sizeof(Node), (byte*)node);
			--m_Count;
		}

		inline ValueType& peak_front() {
			return m_Start->getNext()->getValue();
		}

		inline ValueType& peak_back() {
			return m_Start->getPrev()->getValue();
		}

		inline void push(const T& val) {
			push_front(val);
		}

		inline ValueType pop() {
			return pop_front();
		}

		ValueType pop_front() {
			ValueType result = m_Start->getNext()->getValue();
			remove(m_Start->getNext());
			m_Count--;
			return result;
		}

		ValueType pop_back() {

			ValueType result = m_Start->getPrev()->getValue();
			remove(m_Start->getPrev());
			m_Count--;
			return result;
		}
		
		void push_front(const T& val) {

			Node* newnode = (Node*)allocator(sizeof(Node));
			newnode->m_Val = val;

			Node* oldfront = m_Start->getNext();
			oldfront->m_Prev = newnode;
			m_Start->m_Next = newnode;
			newnode->m_Next = oldfront;
			newnode->m_Prev = m_Start;

			m_Count++;
			return;
		}

		void push_back(const T& val) {
			
			Node* newnode = (Node*)allocator(sizeof(Node));
			newnode->m_Val = val;

			Node* oldback = m_Start->getNext();
			oldback->m_Next = newnode;
			m_Start->m_Prev = newnode;
			newnode->m_Next = m_Start;
			newnode->m_Prev = oldback;

			m_Count++;
			return;
		}

		uint32_t getCapacity() const {
			return m_Count;
		}

		uint32_t getCount() const {
			return m_Count;
		}

		bool isEmpty() const {
			begin() == end();
		}
 
		Node* m_Start;
		uint32_t m_Count;
	};


}