#pragma once

#include <tuple>
#include <cstddef>	//size_t

namespace Common {

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

}
