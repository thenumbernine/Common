#pragma once

// ok putting all metaprograms in "meta" is no longer viable
// here's the metaprograms associated with variadic arg manipulation

#include <cstddef>	//size_t

namespace Common {

/*
get the i'th value from a template variadic arg list of values
https://cplusplus.com/forum/general/241535/#msg1073933
constexpr T variadic_get_v<index, T, Ts...>
T = the type of the seq,
index = which index to get
Ts... = the seq
was thinking of putting the template type as the first arg, so ordrer matched a C++ declaration-assignment  statement, 
 but then put the template type as the 2nd arg, so you can just inject the integer_sequence template args into the variadic_get args
*/

template<size_t i, typename T, T... Ts>
struct variadic_get;

template<size_t i, typename T, T first, T... rest>
struct variadic_get<i, T, first, rest...> {
	static constexpr T value = variadic_get<i-1, T, rest...>::value;
};

template<typename T, T first, T... rest>
struct variadic_get<0, T, first, rest...> {
	static constexpr T value = first;
};

template<size_t i, typename T, T... Ts>
constexpr T variadic_get_v = variadic_get<i, T, Ts...>::value;

// constexpr min(a,b)
//  TODO can I just use std::min(a,b)?  
template<typename T>
constexpr T constexpr_min(T a, T b) {
	return a < b ? a : b;
}

// get the min value of a template variadic index

template<typename T, T... I>
struct variadic_min;

template<typename T, T i1, T... I>
struct variadic_min<T, i1, I...> {
	static constexpr T value = constexpr_min<T>(i1, variadic_min<T, I...>::value);
};
template<typename T, T i>
struct variadic_min<T, i> {
	static constexpr T value = i;
};
template<typename T, T... I>
constexpr T variadic_min_v = variadic_min<T, I...>::value;

// get the *LOCATION* of the min value in a variadic

template<typename T, T... I>
struct variadic_min_loc;

template<typename T, T i1, T... I>
struct variadic_min_loc<T, i1, I...> {
	static constexpr size_t value() {
		constexpr size_t j = variadic_min_loc<T, I...>::value();
		constexpr T ij = variadic_get_v<j, T, I...>;
		return (i1 < ij) ? 0 : j+1;
	}
};

template<typename T, T i>
struct variadic_min_loc<T, i> {
	static constexpr size_t value() { return 0; }
};

#if 0 // how do you make a template specialization to cause a static_assert ... without it always getting hit unless the template type is generated?
template<typename T = void>
static constexpr auto defer_fail() {
	static_assert(false, "can't get the min of an empty list");
}

template<>
struct variadic_min_loc<> {
	static constexpr auto fail() {
		defer_fail<>();
	}
};
#endif

template<typename T, T... I>
constexpr size_t variadic_min_loc_v = variadic_min_loc<T, I...>::value();

}
