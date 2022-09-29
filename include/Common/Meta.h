#pragma once

#include <functional>
#include <tuple>

namespace Common {

//common metaprograms


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
struct tuple_of_same_type_recurse{
    template< typename...Args>
	using type = typename tuple_of_same_type_recurse<T, I-1>::template type<T, Args...>;
};
template <typename T>
struct tuple_of_same_type_recurse<T, 0> {
    template<typename...Args>
	using type = std::tuple<Args...>;
};

template <typename T, size_t I>
using tuple_of_same_type = typename tuple_of_same_type_recurse<T,I>::template type<>;

}
