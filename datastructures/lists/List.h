#pragma once
#include "polymorphic_iterator.h"

namespace IME::Data {

	template<typename T> 
	class List {
	public:
		using iterator = polymorphic_iterator<T>;
		using const_iterator = polymorphic_iterator_const<T>;

		using ValueType = T;
		using ReferenceType = T&;
		using PointerType = T*;

		virtual void add(const ValueType& val) = 0;
		virtual void add(const ValueType& val, uint32_t index) = 0;

		virtual uint32_t getCount() const = 0;
		virtual uint32_t getCapacity() const = 0;

		virtual iterator begin() = 0;
		virtual iterator end() = 0;

		virtual const_iterator begin() const = 0;
		virtual const_iterator end() const = 0;

		virtual bool contains(const T& val) const = 0;
		virtual bool contains(const T& val, bool(*comparator)(const T&, const T&)) const = 0;

		virtual View<iterator> view(uint32_t begin, uint32_t end) = 0;

		virtual void foreach(void(*func)(ValueType&)) = 0;
	};

	template<typename DS>
	class List_impl : public List<DS::template ValueType> {
	public:
		using ValueType = DS::template ValueType;

		using iterator = polymorphic_iterator<DS::template ValueType>;
		using const_iterator = polymorphic_iterator_const<DS::template ValueType>;

		List_impl(const std::initializer_list<ValueType>& in) : m_DS(in) { }
		List_impl() : m_DS() {}

		void foreach(void(*func)(ValueType&)) override {
			for (ValueType& it : m_DS) {
				func(it);
			}
		}

		bool contains(const ValueType& val) const override {
			for (const ValueType& v : m_DS) {
				if (v == val) {
					return true;
				}
			}
			return false;
		}

		bool contains(const ValueType& val, bool(*comparator)(const ValueType&, const ValueType&)) const override {
			for (const ValueType& v : m_DS) {
				if (comparator(val, v)) {
					return true;
				}
			}
			return false;
		}

		uint32_t getCount() const override {
			return m_DS.getCount();
		}
		uint32_t getCapacity() const override {
			return m_DS.getCapacity();
		}

		void add(const ValueType& val) override {
			m_DS.push_back(val);
		}

		void add(const ValueType& val, uint32_t index) override {
			m_DS.insert(val, index);
		}

		DS& getDS() {
			return m_DS;
		}

		View<iterator> view(uint32_t begin, uint32_t end) {			

			View<DS::template iterator> ds_view = m_DS.view(begin, end);
			return View<iterator>(iterator(new DS::template iterator(ds_view.begin())), iterator(new DS::template iterator(ds_view.end())));
		}

		const_iterator begin() const override {
			return const_iterator(new DS::template const_iterator(m_DS.begin()));
		}

		const_iterator end() const override {

			return const_iterator(new DS::template const_iterator(m_DS.end()));
		}

		iterator begin() override {
			return polymorphic_iterator<ValueType>(new DS::template iterator(m_DS.begin()));
		}

		iterator end() override {
			return polymorphic_iterator<ValueType>(new DS::template iterator(m_DS.end()));
		}

	private:
		DS m_DS;
	};
}