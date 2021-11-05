#pragma once
#include "arrays/basetypes/DynamicArray.h"

namespace IME::Data {

	template<typename iterator>
	void swap(iterator& x, iterator& y) {
		
		iterator::template ValueType plh = *x;
		*x = *y;
		*y = plh;
	}

	template<typename iterator>
	void mergesort(iterator begin, const iterator& end, uint32_t size, int(*compare)(const iterator::template ValueType&, const iterator::template ValueType&)) {

		if (size == 2) {
			selectionsort(begin, end, compare);
			return;
		}

		if (size <= 1) {
			return;
		}
		
		DynamicArray<iterator::template ValueType> array1(size / 2);
		DynamicArray<iterator::template ValueType> array2(size - (size / 2));

		IM::Data::copy(array1.begin(), array1.end(), begin);
		IM::Data::copy(array2.begin(), array2.end(), begin + array1.getCapacity());

		std::cout << "array1 : " << array1.getCapacity() << std::endl;
		for (auto it : array1) {
			std::cout << it << std::endl;
		}

		std::cout << "array2 : " << array2.getCapacity() << std::endl;
		for (auto it : array2) {
			std::cout << it << std::endl;
		}

		mergesort(array1.begin(), array1.end(), array1.getCapacity(), compare);
		mergesort(array2.begin(), array2.end(), array2.getCapacity(), compare);

		auto it_1 = array1.begin();
		auto it_2 = array2.begin();

		auto endit_1 = array1.end();
		auto endit_2 = array2.end();
		
		while (begin != end) {

			if (it_2 == endit_2 || (it_1 != endit_1 && 0 > compare(*it_1, *it_2))) {
				*begin = *it_1;
				it_1 += 1;
			}
			else {
				*begin = *it_2;
				it_2 += 1;
			}
			begin++;
		}
		return;
	}

	template<typename iterator>
	void insertionsort(const iterator& begin, const iterator& end, int(*compare)(const iterator::template ValueType&, const iterator::template ValueType&)) {

		iterator current = begin;
		current += 1;

		iterator loop = begin;
		iterator beforeloop = begin;

		while (current != end) {

			loop = current;  
			beforeloop = current;
			loop -= 1;

			iterator::template ValueType currentval = *current;

			while (beforeloop != begin)
				if (0 < compare(*loop, currentval)) {
					swap(loop, beforeloop);
					loop -= 1;
					beforeloop -= 1;
				} else break;
			current += 1;
		}

	}

	template<typename iterator>
	void selectionsort(iterator begin, iterator end, int(*compare)(const iterator::template ValueType&, const iterator::template ValueType&)) {
		
		iterator secondlast = end;
		secondlast -= 1;

		iterator it = begin;
		iterator most_significant = begin;

		while (begin != secondlast) {
			
			it = begin;
			it += 1;
			most_significant = begin;
			
			while (it != end) 
			{
				if (0 > compare(*it, *most_significant)) {
					most_significant = it;
				}
				it += 1;
			}

			swap<iterator>(begin, most_significant);
			begin += 1;
		}


	}

}