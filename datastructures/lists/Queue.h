#pragma once

namespace IME::Data {

	template<typename T>
	class Queue {
	public:
		using ValueType = T;


		virtual void offer(const ValueType& value) = 0;
		virtual ValueType poll() = 0;

		virtual uint32_t getCount() const = 0;
		virtual uint32_t getCapacity() const = 0;

		virtual bool contains(const ValueType& value) const = 0;

	};

	template<typename DS> 
	class Queue_impl : public Queue<DS::template ValueType> {
	public:
		using ValueType = DS::template ValueType;

		void reserve(uint32_t count) {
			m_DS.resize(count);
		}

		void offer(const ValueType& value) override {
			m_DS.push_back(value);
		}

		ValueType poll() override {
			return m_DS.pop_front();
		}

		uint32_t getCount() const override {
			return m_DS.getCount();
		}
		uint32_t getCapacity() const override {
			return m_DS.getCapacity();
		}

		bool contains(const ValueType& value) const override {
			for (const ValueType& it : m_DS) {
				if (it == value) {
					return true;
				}
			}
			return false;
		}

		bool isEmpty() const {
			return m_DS.isEmpty();
		}

	private:
		DS m_DS;
	};



}