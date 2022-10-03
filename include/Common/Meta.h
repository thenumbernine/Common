#pragma once

#include <functional>
#include <tuple>

namespace Common {

//common metaprograms
// ok just calling everything "Common" isn't going to work
// I'm going to need some folders like meta/function, meta/tuple, meta/variadic, meta/sequence


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

template<int index, int end, template<int> class Exec>
struct ForLoop {
	template<typename... InputArgs>
	static bool exec(InputArgs&&... input) {
		if (Exec<index>::exec(std::forward<InputArgs>(input)...)) return true;
		return ForLoop<index+1,end,Exec>::exec(std::forward<InputArgs>(input)...);
	}
};

template<int end, template<int> class Exec>
struct ForLoop<end, end, Exec> {
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
inline typename std::enable_if_t<I == sizeof...(Tp), bool>
TupleForEach(std::tuple<Tp...> const &, FuncT) { return {}; }

template<std::size_t I = 0, typename FuncT, typename... Tp>
inline typename std::enable_if_t<I < sizeof...(Tp), bool>
TupleForEach(std::tuple<Tp...> const & t, FuncT f)
{
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


// get the i'th value from a template variadic arg list of values
// https://cplusplus.com/forum/general/241535/#msg1073933

template<size_t i, size_t... is>
struct variadic_get;

template<size_t i, size_t first, size_t... rest>
struct variadic_get<i, first, rest...> {
	static constexpr size_t value = variadic_get<i-1, rest...>::value;
};

template<size_t first, size_t... rest>
struct variadic_get<0, first, rest...> {
	static constexpr size_t value = first;
};

template<size_t... i>
constexpr size_t variadic_get_v = variadic_get<i...>::value;

static_assert(variadic_get_v<0,1> == 1);
static_assert(variadic_get_v<0,1,2> == 1);
static_assert(variadic_get_v<1,1,2> == 2);

// get the i'th value from an index_sequence

template<size_t i, typename T>
constexpr size_t sequence_get_v = {};

template<size_t i, size_t... I>
constexpr size_t sequence_get_v<i, std::index_sequence<I...>> = variadic_get_v<i, I...>;

// concat index_sequence
//https://devblogs.microsoft.com/oldnewthing/20200625-00/?p=103903

template<typename Seq1, typename Seq>
struct sequence_cat;

template<std::size_t... Ints1, std::size_t... Ints2>
struct sequence_cat<
	std::index_sequence<Ints1...>,
	std::index_sequence<Ints2...>
> {
	using type = std::index_sequence<Ints1..., Ints2...>;
};

template<typename Seq1, typename Seq2>
using sequence_cat_t = typename sequence_cat<Seq1, Seq2>::type;

static_assert(
	std::is_same_v<
		sequence_cat_t<
			std::index_sequence<1>,
			std::index_sequence<2,3>
		>,
		std::index_sequence<1,2,3>
	>
);

// set the i'th value of an index_sequence

template<size_t value, size_t i, typename T>
struct sequence_set;

template<size_t value, size_t i, size_t first, size_t... rest>
struct sequence_set<value, i, std::index_sequence<first, rest...>> {
	using type = sequence_cat_t<
		std::index_sequence<first>,
		typename sequence_set<value, i-1, std::index_sequence<rest...>>::type
	>;
};

template<size_t value, size_t first, size_t... rest>
struct sequence_set<value, 0, std::index_sequence<first, rest...>> {
	using type = sequence_cat_t<
		std::index_sequence<value>,
		std::index_sequence<rest...>
	>;
};

template<size_t value, size_t first>
struct sequence_set<value, 0, std::index_sequence<first>> {
	using type = std::index_sequence<value>;
};

template<size_t value, size_t i, typename T>
using sequence_set_t = typename sequence_set<value, i, T>::type;

static_assert(std::is_same_v<sequence_set_t<3, 0, std::index_sequence<2>>,std::index_sequence<3>>);
static_assert(std::is_same_v<sequence_set_t<3, 0, std::index_sequence<1, 2>>, std::index_sequence<3, 2>>);
static_assert(std::is_same_v<sequence_set_t<3, 1, std::index_sequence<1, 2>>, std::index_sequence<1, 3>>);
static_assert(std::is_same_v<sequence_set_t<3, 0, std::index_sequence<7, 5, 9>>, std::index_sequence<3, 5, 9>>);
static_assert(std::is_same_v<sequence_set_t<3, 1, std::index_sequence<7, 5, 9>>, std::index_sequence<7, 3, 9>>);
static_assert(std::is_same_v<sequence_set_t<3, 2, std::index_sequence<7, 5, 9>>, std::index_sequence<7, 5, 3>>);

// constexpr min(a,b)

constexpr size_t constexpr_min(size_t a, size_t b) {
	return a < b ? a : b;
}

static_assert(constexpr_min(1,2) == 1);
static_assert(constexpr_min(4,3) == 3);

// get the min value of a template variadic index

template<size_t... I>
struct variadic_min;

template<size_t i1, size_t... I>
struct variadic_min<i1, I...> {
	static constexpr size_t value = constexpr_min(i1, variadic_min<I...>::value);
};
template<size_t i>
struct variadic_min<i> {
	static constexpr size_t value = i;
};
template<size_t... I>
constexpr size_t variadic_min_v = variadic_min<I...>::value;

static_assert(variadic_min_v<7> == 7);
static_assert(variadic_min_v<3,8> == 3);
static_assert(variadic_min_v<9,1> == 1);
static_assert(variadic_min_v<2,4,8> == 2);
static_assert(variadic_min_v<5,3,8> == 3);
static_assert(variadic_min_v<9,5,1> == 1);

// index_sequence min value

template<size_t i, typename T>
constexpr size_t sequence_min_v = {};

template<size_t i, size_t... I>
constexpr size_t sequence_min_v<i, std::index_sequence<I...>> = variadic_min_v<i, I...>;

// get the *LOCATION* of the min value in a variadic

template<size_t... I>
struct variadic_min_loc;

template<size_t i1, size_t... I>
struct variadic_min_loc<i1, I...> {
	static constexpr size_t value() {
		constexpr size_t j = variadic_min_loc<I...>::value();
		constexpr size_t ij = variadic_get_v<j, I...>;
		return (i1 < ij) ? 0 : j+1;
	}
};

template<size_t i>
struct variadic_min_loc<i> {
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

template<size_t... I>
constexpr size_t variadic_min_loc_v = variadic_min_loc<I...>::value();

//static_assert(variadic_min_loc_v<> == 0); // should error
static_assert(variadic_min_loc_v<7> == 0);
static_assert(variadic_min_loc_v<3,8> == 0);
static_assert(variadic_min_loc_v<9,1> == 1);
static_assert(variadic_min_loc_v<2,4,8> == 0);
static_assert(variadic_min_loc_v<5,3,8> == 1);
static_assert(variadic_min_loc_v<9,5,1> == 2);

// index_sequence min loc

template<typename T>
constexpr size_t sequence_min_loc_v = {};

template<size_t... I>
constexpr size_t sequence_min_loc_v<std::index_sequence<I...>> = variadic_min_loc_v<I...>;

// sequence get 2nd- to end

template<typename T>
struct sequence_pop_front;

template<size_t i, size_t... I>
struct sequence_pop_front<std::index_sequence<i, I...>> {
	using type = std::index_sequence<I...>;
};

template<size_t i>
struct sequence_pop_front<std::index_sequence<i>> {
	using type = std::index_sequence<>;
};

template<typename T>
using sequence_pop_front_t = typename sequence_pop_front<T>::type;

// sort index_sequence

template<typename T>
struct sequence_sort;

template<size_t i1, size_t... I>
struct sequence_sort<std::index_sequence<i1, I...>> {
	using seq = std::index_sequence<i1, I...>;
	using rest = std::index_sequence<I...>;
	static constexpr auto value() {		//output type is decltype(value())
		constexpr size_t j = variadic_min_loc_v<I...>;	// index in subset, +1 for index in seq
		constexpr size_t ij = variadic_get_v<j, I...>;
		if constexpr (i1 > ij) {
			//set ij in the rest
			using rest_set_i = sequence_set_t<i1, j, rest>;
			// sort the rest
			using rest_set_i_sorted = decltype(sequence_sort<rest_set_i>::value());
			// then prepend the first element
			using sorted = sequence_cat_t<std::index_sequence<ij>, rest_set_i_sorted>;
			return sorted();
		} else {
			// i1 is good, sort I...
			return sequence_cat_t<
				std::index_sequence<i1>,
				decltype(sequence_sort<std::index_sequence<I...>>::value())
			>();
		}
	}
};
template<size_t i>
struct sequence_sort<std::index_sequence<i>> {
	using type = std::index_sequence<i>;
	static constexpr auto value() { return type(); }
};

template<typename T>
using sequence_sort_t = decltype(sequence_sort<T>::value());

static_assert(sequence_min_loc_v<std::index_sequence<3>> == 0);
static_assert(sequence_get_v<sequence_min_loc_v<std::index_sequence<3>>, std::index_sequence<3>> == 3);
static_assert(std::is_same_v<sequence_set_t<3, 0, std::index_sequence<3>>, std::index_sequence<3>>);
static_assert(std::is_same_v<sequence_sort_t<std::index_sequence<1>>, std::index_sequence<1>>);
static_assert(std::is_same_v<sequence_sort_t<std::index_sequence<3,4>>, std::index_sequence<3,4>>);
static_assert(std::is_same_v<sequence_sort_t<std::index_sequence<4,3>>, std::index_sequence<3,4>>);
static_assert(std::is_same_v<sequence_sort_t<std::index_sequence<2,5,8>>, std::index_sequence<2,5,8>>);
static_assert(std::is_same_v<sequence_sort_t<std::index_sequence<2,8,5>>, std::index_sequence<2,5,8>>);
static_assert(std::is_same_v<sequence_sort_t<std::index_sequence<5,2,8>>, std::index_sequence<2,5,8>>);
static_assert(std::is_same_v<sequence_sort_t<std::index_sequence<5,8,2>>, std::index_sequence<2,5,8>>);
static_assert(std::is_same_v<sequence_sort_t<std::index_sequence<8,2,5>>, std::index_sequence<2,5,8>>);
static_assert(std::is_same_v<sequence_sort_t<std::index_sequence<8,5,2>>, std::index_sequence<2,5,8>>);



}
