#pragma once

#include "Common/Sequence.h"	// Tuple/Seq stuff should either go here or in Common/Sequence.h
#include <tuple>
#include <cstddef>	//size_t
#include <utility>	//integer_sequence

namespace Common {


template<typename... T>
using tuple_cat_t = decltype(std::tuple_cat(std::declval<T>()...));

//https://stackoverflow.com/a/15412010
template< 
	typename Tuple, 
	std::size_t N,
	typename T,
	typename Indices = typename std::make_index_sequence<std::tuple_size_v<Tuple>>
> struct element_replace;
template<typename... Ts, std::size_t N, typename T, std::size_t... Ns>
struct element_replace<std::tuple< Ts... >, N, T, std::index_sequence< Ns... >> {
    using type = std::tuple<typename std::conditional_t<Ns == N, T, Ts>...>;
};
template<typename Tuple, std::size_t N, typename T>
using element_replace_t = typename element_replace<Tuple, N, T>::type;

static_assert(std::is_same_v<
	element_replace_t<std::tuple<int, char*, double>, 1, float>,
	std::tuple<int, float, double>
>);

// https://stackoverflow.com/a/14852674
template<size_t I, typename T>
struct tuple_remove {};
template<typename T, typename... Ts>
struct tuple_remove<0, std::tuple<T, Ts...>> {
	typedef std::tuple<Ts...> type;
};
template<size_t I, typename T, typename... Ts>
struct tuple_remove<I, std::tuple<T, Ts...>> {
	using type = decltype(tuple_cat(
		std::declval<std::tuple<T>>(),
		std::declval<typename tuple_remove<I - 1, std::tuple<Ts...>>::type>()
	));
};
template<size_t I, typename T>
using tuple_remove_t = typename tuple_remove<I, T>::type;

template<typename T>
using tuple_remove_last_t = tuple_remove_t<std::tuple_size_v<T>-1, T>;


template<typename Tuple, typename I, I... is>
using TupleGetVariadic = std::tuple< std::tuple_element_t<is, Tuple> ... >;

template<typename Tuple, typename Seq>
struct TupleGetSeqImpl;
template<typename Tuple, typename I, I... is>
struct TupleGetSeqImpl<Tuple, std::integer_sequence<I, is...>> {
	using type = TupleGetVariadic<Tuple, I, is...>;
};
template<typename Tuple, typename Seq>
using TupleGetSeq = typename TupleGetSeqImpl<Tuple, Seq>::type; 

template<typename Tuple, size_t from, size_t to>
using tuple_subset_t = TupleGetSeq<
	Tuple,
	make_index_range<
		from,
		std::min(std::tuple_size_v<Tuple>, std::max(from,to))
	>
>;

//insert Ins into Src at 'index'
template<typename Src, int index, typename Ins>
using tuple_insert_t = tuple_cat_t<
	tuple_subset_t<Src, 0, index>,
	Ins,
	tuple_subset_t<Src, index, std::tuple_size_v<Src>>
>;

// apply tuple arguments to a templated type
template<template<typename...> typename F, typename TupleOfArgs>
struct tuple_apply_impl;
template<template<typename...> typename F, typename... Args>
struct tuple_apply_impl<F, std::tuple<Args...>> {
	using type = F<Args...>;
};
template<template<typename...> typename F, typename TupleOfArgs>
using tuple_apply_t = typename tuple_apply_impl<F, TupleOfArgs>::type;


template<typename What, typename WhereTuple>
struct tuple_find;
template<typename What, typename Where1, typename... Wheres>
struct tuple_find<What, std::tuple<Where1, Wheres...>> {
	static constexpr auto nextvalue = tuple_find<What, std::tuple<Wheres...>>::value;
	static constexpr auto value = 
		std::is_same_v<What, Where1>
		? 0
		: (nextvalue == -1 ? -1 : nextvalue + 1);
};
template<typename What>
struct tuple_find<What, std::tuple<>> {
	static constexpr auto value = -1;
};
template<typename What, typename WhereTuple>
static constexpr int tuple_find_v = tuple_find<What, WhereTuple>::value;



//https://stackoverflow.com/a/38894158
// but i switched template args T and I so they match array and my own vec
template <typename T, size_t I>
struct tuple_rep_impl{
    template< typename...Args>
	using type = typename tuple_rep_impl<T, I-1>::template type<T, Args...>;
};
template <typename T>
struct tuple_rep_impl<T, 0> {
    template<typename...Args>
	using type = std::tuple<Args...>;
};
template <typename T, size_t I>
using tuple_rep_t = typename tuple_rep_impl<T,I>::template type<>;


// https://stackoverflow.com/a/6894436/2714073
// return 'true' to stop early
template<std::size_t I = 0, typename FuncT, typename... Tp>
requires(I == sizeof...(Tp))
bool TupleForEach(std::tuple<Tp...> const &, FuncT) { return {}; }

template<std::size_t I = 0, typename FuncT, typename... Tp>
requires(I < sizeof...(Tp))
bool TupleForEach(std::tuple<Tp...> const & t, FuncT f) {
	if (f(std::get<I>(t), I)) return true;
	return TupleForEach<I + 1, FuncT, Tp...>(t, f);
}


// Tuple/Tuple, Tuple/Seq, Seq/Tuple, Seq/Seq mapping via template arg
// put these in their own header?

template<typename T, template<typename> typename F>
struct TupleTypeMapImpl;
template<typename T, typename... Ts, template<typename> typename F>
struct TupleTypeMapImpl<std::tuple<T, Ts...>, F> {
	using type = tuple_cat_t<
		std::tuple<F<T>>,
		typename TupleTypeMapImpl<std::tuple<Ts...>, F>::type
	>;
};
template<template<typename> typename F>
struct TupleTypeMapImpl<std::tuple<>, F> {
	using type = std::tuple<>;
};
template<typename T, template<typename> typename F>
using TupleTypeMap = typename TupleTypeMapImpl<T, F>::type;

//apply successive types
template<typename T, template<typename> typename... Fs>
struct TupleTypeMapsImpl;
template<typename T, template<typename> typename F, template<typename> typename... Fs>
struct TupleTypeMapsImpl<T, F, Fs...> {
	using type = typename TupleTypeMapsImpl<TupleTypeMap<T, F>, Fs...>::type;
};
template<typename T>
struct TupleTypeMapsImpl<T> {
	using type = T;
};
template<typename T, template<typename> typename... Fs>
using TupleTypeMaps = typename TupleTypeMapsImpl<T, Fs...>::type;

// tuple to sequence map
// can't seem to pass "template<typename> int F", so F must be a class with a ::value whose type matches integer_sequence type I
template<typename I, typename Tuple, template<typename> typename F>
struct TupleToSeqMapImpl;
template<typename I, typename T, typename... Ts, template<typename> typename F>
struct TupleToSeqMapImpl<I, std::tuple<T, Ts...>, F> {
	using type = seq_cat_t<
		std::integer_sequence<I, F<T>::value>,
		typename TupleToSeqMapImpl<I, std::tuple<Ts...>, F>::type
	>;
};
template<typename I, template<typename> typename F>
struct TupleToSeqMapImpl<I, std::tuple<>, F> {
	using type = std::integer_sequence<I>;
};
template<typename I, typename T, template<typename> typename F>
using TupleToSeqMap = typename TupleToSeqMapImpl<I, T, F>::type;


template<typename Seq, template<typename Seq::value_type> typename F>
struct SeqToTupleMapImpl;
template<typename I, I i1, I... is, template<I> typename F>
struct SeqToTupleMapImpl<std::integer_sequence<I, i1, is...>, F> {
	using type = tuple_cat_t<
		std::tuple<F<i1>>,
		typename SeqToTupleMapImpl<std::integer_sequence<I, is...>, F>::type
	>;
};
template<typename I, template<I> typename F>
struct SeqToTupleMapImpl<std::integer_sequence<I>, F> {
	using type = std::tuple<>;
};
template<typename Seq, template<typename Seq::value_type> typename F>
using SeqToTupleMap = typename SeqToTupleMapImpl<Seq, F>::type;

// assumes F<I>::value produces Seq::value_type
template<typename Seq, template<typename Seq::value_type> typename F>
struct SeqToSeqMapImpl;
template<typename I, I i1, I... is, template<I> typename F>
struct SeqToSeqMapImpl<std::integer_sequence<I, i1, is...>, F> {
	using type = seq_cat_t<
		std::integer_sequence<I, F<i1>::value>,
		typename SeqToSeqMapImpl<std::integer_sequence<I, is...>, F>::type
	>;
};
template<typename I, template<I> typename F>
struct SeqToSeqMapImpl<std::integer_sequence<I>, F> {
	using type = std::integer_sequence<I>;
};
template<typename Seq, template<typename Seq::value_type> typename F>
using SeqToSeqMap = typename SeqToSeqMapImpl<Seq, F>::type;


// F<T>::value is a bool for yes or no to add to 'has' or 'hasnot'
template<typename Tuple, typename indexSeq, template<typename> typename F>
struct tuple_get_filtered_indexes;
template<
	typename T, typename... Ts,
	int i1, int... is,
	template<typename> typename F
>
struct tuple_get_filtered_indexes<
	std::tuple<T, Ts...>, 
	std::integer_sequence<int, i1, is...>,
	F
> {
	using next = tuple_get_filtered_indexes<
		std::tuple<Ts...>, 
		std::integer_sequence<int, is...>,
		F
	>;
	static constexpr auto value() {
		if constexpr (F<T>::value) {
			using has = seq_cat_t<
				std::integer_sequence<int, i1>,
				typename next::has
			>;
			using hasnot = typename next::hasnot;
			return (std::pair<has, hasnot>*)nullptr;
		} else {
			using has = typename next::has;
			using hasnot = seq_cat_t<
				std::integer_sequence<int, i1>,
				typename next::hasnot
			>;
			return (std::pair<has, hasnot>*)nullptr;
		}
	}
	using result = typename std::remove_pointer_t<decltype(value())>;
	using has = typename result::first_type;
	using hasnot = typename result::second_type;
};
template<template<typename> typename F>
struct tuple_get_filtered_indexes<std::tuple<>, std::integer_sequence<int>, F> {
	using has = std::integer_sequence<int>;
	using hasnot = std::integer_sequence<int>;
};
template<typename Tuple, template<typename> typename F>
using tuple_get_filtered_indexes_t = tuple_get_filtered_indexes<
	Tuple,
	std::make_integer_sequence<int, std::tuple_size_v<Tuple>>,
	F
>;


}
