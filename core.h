#pragma once

#ifdef IME_DEBUG
	#include<intrin.h>
	#define IME_DEBUG_ASSERT_BREAK(x, message) if(!(x)) { __debugbreak(); }
	#define IME_DEBUG_BREAK() { __debugbreak(); }
#elif
	#define IME_DEBUG_ASSERT_BREAK(x, message) if(!(x)) {  }
	#define IME_DEBUG_BREAK()
#endif //IME_DEBUG

#include <stdint.h>

#define IME_KILOBYTE(x) (IME::uint64)x * 1024
#define IME_MEGABYTE(x) IME_KILOBYTE((IME::uint64)x) * 1024
#define IME_GIGABYTE(x) IME_MEGABYTE((IME::uint64)x) * 1024
#define IME_TERRABYTE(x) IME_GIGABYTE((IME::uint64)x) * 1024

#define global_var static;
#define internal static;
#define local_persist static;

#define PI32 3.14159265359f


namespace IME {

	typedef uint64_t bool64;
	typedef uint32_t bool32;
	typedef uint16_t bool16;
	typedef uint8_t bool8;

	typedef float real32;
	typedef double real64;

	typedef int8_t byte;
	typedef int8_t int8;
	typedef int16_t int16;
	typedef int32_t int32;
	typedef int64_t int64;

	typedef uint8_t ubyte;
	typedef uint8_t uint8;
	typedef uint16_t uint16;
	typedef uint32_t uint32;
	typedef uint64_t uint64;

	typedef size_t sizeptr;

}
