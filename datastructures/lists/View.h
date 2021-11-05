#pragma once

namespace IME::Data {

	template<typename it>
	class View {
	public:

		View(it&& begin, it&& end) : m_End(std::move(end)), m_Begin(std::move(begin)) {

		}

		View(const it& begin, const it& end) : m_End(std::move(end)), m_Begin(std::move(begin)) {

		}

		View(View&& other) : m_End(std::move(other.m_End)), m_Begin(std::move(other.m_Begin)) {

		}

		inline const it& begin() {
			return m_Begin;
		}

		inline const it& end() {
			return m_End;
		}

	private:
		it m_End;
		it m_Begin;
	};



}