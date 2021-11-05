#pragma once
#include "iterators/iterator_C.h"

namespace IME::Data {

	template<typename T> 
	inline void move(iterator_C_base<T> begin_dst, iterator_C_base<T> end_dst, iterator_C_base<T> begin_src) {
		memcpy(&(*begin_dst), &(*begin_src), begin_dst.diff(end_dst));
	}

	template<typename it_dst, typename it_src>
	inline void copy(it_dst begin_dst, it_dst end_dst, it_src begin_src) {
		for (begin_dst; begin_dst != end_dst; begin_dst++) {
			*begin_dst = *begin_src;
			begin_src++;
		}
	}

	template<typename it_dst, typename it_src>
	inline void copy_reverse(it_dst begin_dst, it_dst end_dst, it_src end_src) {
		end_dst--;
		begin_dst--;
		for (end_dst; begin_dst != end_dst; end_dst--) {
			end_src--;
			*end_dst = *end_src;
		}
	}

	template<typename it_dst, typename it_src>
	inline void move(it_dst begin_dst, it_dst end_dst, it_src begin_src) {
		for (begin_dst; begin_dst != end_dst; begin_dst++) {
			*begin_dst = *begin_src;
			begin_src++;
		}
	}




}