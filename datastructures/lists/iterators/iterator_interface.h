#pragma once

namespace IME::Data {

	template<typename T>
	class iterator_interface {
	public:

		using ValueType = T;
		using PointerType = ValueType*;
		using ReferenceType = ValueType&;

		virtual void operator+=(uint32_t add) = 0;
		virtual void operator-=(uint32_t sub) = 0;

		virtual ValueType& operator[](uint32_t index) const = 0;
		virtual PointerType operator->() const = 0;
		virtual ValueType& operator*() const = 0;

		virtual bool operator==(iterator_interface* other) const {return false; }
		virtual bool operator!=(iterator_interface* other) const {return true; }

		virtual void operator=(iterator_interface<T>* other) {

		}

		virtual iterator_interface* copy() { return nullptr; };
	};

	template<typename T>
	class iterator_interface_const {
	public:

		using ValueType = T;
		using PointerType = ValueType*;
		using ReferenceType = ValueType&;

		virtual void operator+=(uint32_t add) = 0;
		virtual void operator-=(uint32_t sub) = 0;

		virtual const ValueType& operator[](uint32_t index) const = 0;
		virtual const PointerType operator->() const = 0;
		virtual const ValueType& operator*() const = 0;

		virtual bool operator==(iterator_interface_const* other) const = 0;
		virtual bool operator!=(iterator_interface_const* other) const = 0;

		virtual void operator=(iterator_interface_const<T>* other) {

		}

		virtual iterator_interface_const* copy() = 0;
	};


}