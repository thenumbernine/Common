#pragma once

#include "Common/Variadic.h"
#include <utility>		//integer_sequence<>

namespace Common {

// begin https://codereview.stackexchange.com/a/64702/265778

namespace details {
	template<typename Int, typename, Int Begin, bool Increasing>
	struct integer_range_impl;

	template<typename Int, Int... N, Int Begin>
	struct integer_range_impl<Int, std::integer_sequence<Int, N...>, Begin, true> {
		using type = std::integer_sequence<Int, N+Begin...>;
	};

	template<typename Int, Int... N, Int Begin>
	struct integer_range_impl<Int, std::integer_sequence<Int, N...>, Begin, false> {
		using type = std::integer_sequence<Int, Begin-N...>;
	};
}

template<typename Int, Int Begin, Int End>
using make_integer_range = typename details::integer_range_impl<
	Int,
	std::make_integer_sequence<Int, (Begin<End) ? End-Begin : Begin-End>,
	Begin,
	(Begin < End)
>::type;

template<std::size_t Begin, std::size_t End>
using make_index_range = make_integer_range<std::size_t, Begin, End>;

// end https://codereview.stackexchange.com/a/64702/265778

// get the i'th value from an index_sequence
// I'm putting that seq-type last so I can default use the integer_sequence type

template<size_t i, typename T, typename R = typename T::value_type>
constexpr R seq_get_v = {};

template<size_t i, typename T, T... I>
constexpr T seq_get_v<i, std::integer_sequence<T, I...>> = variadic_get_v<i, T, I...>;

// concat index_sequence
//https://devblogs.microsoft.com/oldnewthing/20200625-00/?p=103903

// because there's a case that Seqs... is zero-sized, you do have to provide a default type.
template<typename Int, typename... Seqs>
struct seq_cat;
template<typename Int>
struct seq_cat<Int> {
	using type = std::integer_sequence<Int>;
};
template<typename Int, Int... Ints1>
struct seq_cat<
	Int,
	std::integer_sequence<Int, Ints1...>
> {
	using type = std::integer_sequence<Int, Ints1...>;
};
template<typename Int, Int... Ints1, Int... Ints2>
struct seq_cat<
	Int,
	std::integer_sequence<Int, Ints1...>,
	std::integer_sequence<Int, Ints2...>
> {
	using type = std::integer_sequence<Int, Ints1..., Ints2...>;
};
template<typename Int, typename Seq1, typename Seq2, typename... Seqs>
struct seq_cat<Int, Seq1, Seq2, Seqs...> {
	using type = typename seq_cat<
		Int,
		typename seq_cat<Int, Seq1, Seq2>::type,
		typename seq_cat<Int, Seqs...>::type
	>::type;
};
template<typename Int, typename... Seqs>
using seq_cat_t = typename seq_cat<Int, Seqs...>::type;

// set the i'th value of an index_sequence

template<typename R, R value, size_t i, typename T>
struct seq_set;

template<typename R, R value, size_t i, R first, R... rest>
struct seq_set<R, value, i, std::integer_sequence<R, first, rest...>> {
	using type = seq_cat_t<
		R,
		std::integer_sequence<R, first>,
		typename seq_set<R, value, i-1, std::integer_sequence<R, rest...>>::type
	>;
};

template<typename R, R value, R first, R... rest>
struct seq_set<R, value, 0, std::integer_sequence<R, first, rest...>> {
	using type = seq_cat_t<
		R,
		std::integer_sequence<R, value>,
		std::integer_sequence<R, rest...>
	>;
};

template<typename R, R value, R first>
struct seq_set<R, value, 0, std::integer_sequence<R, first>> {
	using type = std::integer_sequence<R, value>;
};

// for value's type to be dependent on T, value has to go last (unless you put the index last?
// seq_set_t<seq, i, value> <=> seq[i] = value
template<typename T, size_t i, typename T::value_type value>
using seq_set_t = typename seq_set<typename T::value_type, value, i, T>::type;

// index_sequence min value

template<typename T>
constexpr typename T::value_type seq_min_v = {};

template<typename T, T... I>
constexpr T seq_min_v<std::integer_sequence<T, I...>> = variadic_min_v<T, I...>;

// index_sequence min loc

template<typename T>
constexpr size_t seq_min_loc_v = {};

template<typename T, T... I>
constexpr size_t seq_min_loc_v<std::integer_sequence<T, I...>> = variadic_min_loc_v<T, I...>;

// seq get 2nd- to end

template<typename T>
struct seq_pop_front;

template<typename T, T i, T... I>
struct seq_pop_front<std::integer_sequence<T, i, I...>> {
	using type = std::integer_sequence<T, I...>;
};

template<typename T, T i>
struct seq_pop_front<std::integer_sequence<T, i>> {
	using type = std::integer_sequence<T>;
};

template<typename T>
using seq_pop_front_t = typename seq_pop_front<T>::type;

// sort index_sequence

template<typename T>
struct seq_sort;

template<typename R, R i1, R... I>
struct seq_sort<std::integer_sequence<R, i1, I...>> {
	using seq = std::integer_sequence<R, i1, I...>;
	using rest = std::integer_sequence<R, I...>;
	static constexpr auto value() {		//output type is decltype(value())
		constexpr size_t j = variadic_min_loc_v<R, I...>;	// index in subset, +1 for index in seq
		constexpr R ij = variadic_get_v<j, R, I...>;
		if constexpr (i1 > ij) {
			//set ij in the rest
			using rest_set_i = seq_set_t<rest, j, i1>;
			// sort the rest
			using rest_set_i_sorted = decltype(seq_sort<rest_set_i>::value());
			// then prepend the first element
			using sorted = seq_cat_t<R, std::integer_sequence<R, ij>, rest_set_i_sorted>;
			return sorted();
		} else {
			// i1 is good, sort I...
			return seq_cat_t<
				R,
				std::integer_sequence<R, i1>,
				decltype(seq_sort<std::integer_sequence<R, I...>>::value())
			>();
		}
	}
};
template<typename R, R i>
struct seq_sort<std::integer_sequence<R, i>> {
	using type = std::integer_sequence<R, i>;
	static constexpr auto value() { return type(); }
};

template<typename T>
using seq_sort_t = decltype(seq_sort<T>::value());

// seq compile time for loop

template<typename T>
struct for_seq_runtime_impl;

template<typename T, T i, T... I>
struct for_seq_runtime_impl<std::integer_sequence<T, i, I...>> {
	template<typename F>
	static constexpr bool exec(F f) {
		if (f(i)) return true;
		return for_seq_runtime_impl<std::integer_sequence<T,I...>>::exec(f);
	}
};

template<typename T, T i>
struct for_seq_runtime_impl<std::integer_sequence<T, i>> {
	template<typename F>
	static constexpr bool exec(F f) {
		return f(i);
	}
};

// T is the sequence type and has to go first
template<typename T, typename F>
constexpr bool for_seq_runtime(F f) {
	return for_seq_runtime_impl<T>::template exec<F>(f);
}

// seq compile time for loop with template arg

template<typename T, template<int> typename F, typename... Args>
struct for_seq_impl;

template<typename T, T i, T... I, template<int> typename F, typename... Args>
struct for_seq_impl<std::integer_sequence<T, i, I...>, F, Args...> {
	static constexpr bool exec(Args&& ... args) {
		if (F<i>::exec(std::forward<Args>(args)...)) return true;
		return for_seq_impl<std::integer_sequence<T,I...>, F, Args...>::exec(std::forward<Args>(args)...);
	}
};

template<typename T, template<int> typename F, typename... Args>
struct for_seq_impl<std::integer_sequence<T>, F, Args...> {
	static constexpr bool exec(Args && ... args) {
		return false;
	}
};

// T is the sequence type and has to go first
template<typename T, template<int> typename F, typename ... Args>
constexpr bool for_seq(Args && ... args) {
	return for_seq_impl<T, F, Args...>::exec(std::forward<Args>(args)...);
}

// sequence reverse 

template<typename T>
struct seq_reverse_impl;
template<typename T, T first, T... Rest>
struct seq_reverse_impl<std::integer_sequence<T, first, Rest...>> {
	using seq_first = std::integer_sequence<T, first>;
	using rest = std::integer_sequence<T, Rest...>;
	using rest_reversed = typename seq_reverse_impl<rest>::type;
	using type = seq_cat_t<T, rest_reversed, seq_first>;
};
template<typename T, T i1, T i2>
struct seq_reverse_impl<std::integer_sequence<T, i1, i2>> {
	using type = std::integer_sequence<T, i2, i1>;
};
template<typename T, T i>
struct seq_reverse_impl<std::integer_sequence<T, i>> {
	using type = std::integer_sequence<T, i>;
};
template<typename T>
using seq_reverse_t = typename seq_reverse_impl<T>::type;


// TODO for all operations 
//https://stackoverflow.com/a/55247213
template<typename T, T... Args>
constexpr T seq_plus(std::integer_sequence<T, Args...> = {}) {
	return (Args + ... + (0));
}
template<typename T, T... Args>
constexpr T seq_multiplies(std::integer_sequence<T, Args...> = {}) {
	return (Args * ... * (1));
}
template<typename T, T... Args>
constexpr T seq_logical_and(std::integer_sequence<T, Args...> = {}) {
	return (Args && ... && (true));
}

}
