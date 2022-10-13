#pragma once

#include <functional>	//function<>
#include <tuple>		//tuple<>
#include <utility>		//integer_sequence<>

namespace Common {

//common metaprograms
// ok just calling everything "Common" isn't going to work
// I'm going to need some folders like meta/function, meta/tuple, meta/variadic, meta/seq


//function parameters

template<typename Return, typename... ArgList>
struct Function;

template<typename Return_, typename... ArgList>
struct Function<Return_(ArgList...)> {
	using Type = Return_(ArgList...);
	using Return = Return_;
	using Args = std::tuple<ArgList...>;
	static constexpr auto numArgs = std::tuple_size_v<Args>;
	
	template<int index>
	using Arg = std::tuple_element_t<index, Args>;

	// running out of naming conventions to use ... Common::Function is my wrapper ... Type is for the C-function-type ... FuncType is for the C++ function<> type ...
	using FuncType = std::function<Type>;
};

//building a Function from a return type and tuple type to represent its arg type list

template<typename Return, typename TupleArgList>
struct FunctionFromTupleArgsImpl {
};

template<typename Return_, typename... ArgList>
struct FunctionFromTupleArgsImpl<Return_, std::tuple<ArgList...>> {
	using type = Function<Return_(ArgList...)>;
};

template<typename Return, typename TupleArgList>
using FunctionFromTupleArgs = typename FunctionFromTupleArgsImpl<Return, TupleArgList>::type;

//for loop - compile-time indexes and execution of runtime code

template<int index, int end, template<int> class Exec, int step = 1>
struct ForLoop {
	template<typename... InputArgs>
	static bool exec(InputArgs&&... input) {
		if (Exec<index>::exec(std::forward<InputArgs>(input)...)) return true;
		return ForLoop<index+step,end,Exec,step>::exec(std::forward<InputArgs>(input)...);
	}
};

template<int end, template<int> class Exec, int step>
struct ForLoop<end, end, Exec, step> {
	template<typename... InputArgs>
	static bool exec(InputArgs&&... input) {
		return false;
	}
};

//https://stackoverflow.com/a/43211852
template<typename T> struct MemberPointer {};
template<typename Class_, typename FieldType_>
struct MemberPointer<FieldType_ Class_::*> {
	using Class = Class_;
	using FieldType = FieldType_;
};

// member method pointer, but only for templated return and void() args
// TODO vararg handle all args

template<typename T> struct MemberMethodPointer {};

template<typename Class_, typename Return_, typename... Args>
struct MemberMethodPointer<Return_ (Class_::*)(Args...)> {
	static constexpr bool is_const = false;
	static constexpr bool is_noexcept = false;
	using Class = Class_;
	using Return = Return_;
	//"FuncType" is what I'm using in Function above for the std::function<> wrapper type
	// in Function above I'm just calling "Type" the C function type, but here Type should be the member method type ...
	// so I'll call this "CFunc"
	using CFunc = Return_(Args...);
};
template<typename Class_, typename Return_, typename... Args>
struct MemberMethodPointer<Return_ (Class_::*)(Args...) noexcept> {
	static constexpr bool is_const = false;
	static constexpr bool is_noexcept = true;
	using Class = Class_;
	using Return = Return_;
	using CFunc = Return_(Args...);
};
template<typename Class_, typename Return_, typename... Args>
struct MemberMethodPointer<Return_ (Class_::*)(Args...) const> {
	static constexpr bool is_const = true;
	static constexpr bool is_noexcept = false;
	using Class = Class_;
	using Return = Return_;
	using CFunc = Return_(Args...);
};
template<typename Class_, typename Return_, typename... Args>
struct MemberMethodPointer<Return_ (Class_::*)(Args...) const noexcept> {
	static constexpr bool is_const = true;
	static constexpr bool is_noexcept = true;
	using Class = Class_;
	using Return = Return_;
	using CFunc = Return_(Args...);
};

// function from lambda

// get a Function<> above for the signature of a lambda based on its operator() member method
template<typename Lambda>
using FunctionFromLambda = Function<typename MemberMethodPointer<decltype(&Lambda::operator())>::CFunc>;

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

//https://stackoverflow.com/a/44215296
/*
ex:
From = std::vector<std::string>
From::value_type = std::string
To = std::vector<size_t>
To::value_type = size_t
*/
template<typename From, typename To>
To mapElems(
	From const & from,
	std::function<typename To::value_type(typename From::value_type)> f
) {
	To to;
#if 0	//won't work with map
	std::transform(from.cbegin(), from.cend(), std::back_inserter(to), f);
#endif
#if 0	//won't work with vector
	for (auto const & fromElem : from) {
		to.insert(f(fromElem));
	}
#endif
#if 1
	for (auto const & fromElem : from) {
		to.insert(to.end(), f(fromElem));
	}
#endif
	return to;
}

//There are probably better things for me to do with my time ...

template<typename From, typename To>
To mapElemsToMemberField(
	From const & from,
	typename To::value_type From::value_type::*fromElemField
) {
	To to;
#if 0	//won't work with map
	std::transform(from.cbegin(), from.cend(), std::back_inserter(to),
		[fromElemField](typename From::value_type const & fromElem) -> typename To::value_type {
			return fromElem.*fromElemField;
		}
	);
#endif
#if 0
	for (auto const & fromElem : from) {
		to.insert(fromElem.*fromElemField);
	}
#endif
#if 1
	for (auto const & fromElem : from) {
		to.insert(to.end(), fromElem.*fromElemField);
	}
#endif
	return to;
}

template<typename From, typename To>
To mapElemsToMemberMethod(
	From const & from,
	typename To::value_type (From::value_type::*fromElemMethod)() const
) {
	To to;
#if 0	//won't work with map
	std::transform(from.cbegin(), from.cend(), std::back_inserter(to),
		[fromElemMethod](typename From::value_type const & fromElem) -> typename To::value_type {
			return (fromElem.*fromElemMethod)();
		}
	);
#endif
#if 0
	for (auto const & fromElem : from) {
		to.insert((fromElem.*fromElemMethod)());
	}
#endif
#if 1
	for (auto const & fromElem : from) {
		to.insert(to.end(), (fromElem.*fromElemMethod)());
	}
#endif
	return to;
}


// map function in C++
// https://stackoverflow.com/a/30534744

template <
	template <class...> class Container,
	class Transform,
	class ContainerT
	/*, class... ContainerParams // breaks in g++ < 12.1.0 */
>
auto mapValues(
	Container<ContainerT/*, ContainerParams...*/> const & container,
	Transform && transform
) {
	using DestT = std::result_of_t<Transform(ContainerT const&)>;
	Container<DestT/*, ContainerParams...*/> res;
	std::transform(
		std::begin(container),
		std::end(container),
		std::inserter(res, std::end(res)),
		std::forward<Transform>(transform)
	);
	return res;
}

// TODO I could use MemberPointer like I'm doing in 'mapValuesToMemberMethod' but meh
template<
	template <typename...> typename Container,
	typename FromElem,
	typename ToElem
>
Container<ToElem> mapValuesToMemberField(
	Container<FromElem> const & from,
	ToElem FromElem::*fromElemField
) {
	return mapValues(from, [fromElemField](
		FromElem const & fromElem
	) -> ToElem {
		return fromElem.*fromElemField;
	});
}

template<
	template<typename...> typename Container,
	typename MemberMethod
>
auto mapValuesToMemberMethod(
	Container<
		typename MemberMethodPointer<MemberMethod>::Class
	> const & from,
	MemberMethod fromElemMethod
) {
	return mapValues(from, [fromElemMethod](
		typename MemberMethodPointer<MemberMethod>::Class const & fromElem
	) -> typename MemberMethodPointer<MemberMethod>::Return {
		return (fromElem.*fromElemMethod)();
	});
}

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

// get the i'th value from an index_sequence
// I'm putting that seq-type last so I can default use the integer_sequence type

template<size_t i, typename T, typename R = typename T::value_type>
constexpr R seq_get_v = {};

template<size_t i, typename T, T... I>
constexpr T seq_get_v<i, std::index_sequence<I...>> = variadic_get_v<i, T, I...>;

// concat index_sequence
//https://devblogs.microsoft.com/oldnewthing/20200625-00/?p=103903

template<typename Seq1, typename Seq>
struct seq_cat;

template<typename T, T... Ints1, T... Ints2>
struct seq_cat<
	std::integer_sequence<T, Ints1...>,
	std::integer_sequence<T, Ints2...>
> {
	using type = std::integer_sequence<T, Ints1..., Ints2...>;
};

template<typename Seq1, typename Seq2>
using seq_cat_t = typename seq_cat<Seq1, Seq2>::type;

// set the i'th value of an index_sequence

template<typename R, R value, size_t i, typename T>
struct seq_set;

template<typename R, R value, size_t i, R first, R... rest>
struct seq_set<R, value, i, std::integer_sequence<R, first, rest...>> {
	using type = seq_cat_t<
		std::integer_sequence<R, first>,
		typename seq_set<R, value, i-1, std::integer_sequence<R, rest...>>::type
	>;
};

template<typename R, R value, R first, R... rest>
struct seq_set<R, value, 0, std::integer_sequence<R, first, rest...>> {
	using type = seq_cat_t<
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

// constexpr min(a,b)

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

// index_sequence min value

template<typename T>
constexpr typename T::value_type seq_min_v = {};

template<typename T, T... I>
constexpr T seq_min_v<std::integer_sequence<T, I...>> = variadic_min_v<T, I...>;

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
			using sorted = seq_cat_t<std::integer_sequence<R, ij>, rest_set_i_sorted>;
			return sorted();
		} else {
			// i1 is good, sort I...
			return seq_cat_t<
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
struct for_seq_impl;

template<typename T, T i, T... I>
struct for_seq_impl<std::integer_sequence<T, i, I...>> {
	template<typename F>
	static constexpr bool exec(F f) {
		if (f(i)) return true;
		return for_seq_impl<std::integer_sequence<T,I...>>::exec(f);
	}
};

template<typename T, T i>
struct for_seq_impl<std::integer_sequence<T, i>> {
	template<typename F>
	static constexpr bool exec(F f) {
		return f(i);
	}
};

// T is the sequence type and has to go first
template<typename T, typename F>
constexpr bool for_seq(F f) {
	return for_seq_impl<T>::template exec<F>(f);
}

// sequence reverse 

template<typename T>
struct seq_reverse_impl;
template<typename T, T first, T... Rest>
struct seq_reverse_impl<std::integer_sequence<T, first, Rest...>> {
	using seq_first = std::integer_sequence<T, first>;
	using rest = std::integer_sequence<T, Rest...>;
	using rest_reversed = typename seq_reverse_impl<rest>::type;
	using type = Common::seq_cat_t<rest_reversed, seq_first>;
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

}
