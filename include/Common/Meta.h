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
};

//building a Function from a return type and tuple type to represent its arg type list 

template<typename Return, typename TupleArgList>
struct FunctionWithTupleArgs {
};

template<typename Return_, typename... ArgList>
struct FunctionWithTupleArgs<Return_, std::tuple<ArgList...>> {
	using type = Function<Return_(ArgList...)>;
};

template<typename Return, typename TupleArgList>
using FunctionWithTupleArgs_t = typename FunctionWithTupleArgs<Return, TupleArgList>::type;

//building a Function from a Lambda

template<typename Lambda>
using FunctionFromLambda = Common::Function<
	typename std::remove_pointer<decltype(+Lambda())>::type
>;

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
template<typename ClassType_, typename FieldType_>
struct MemberPointer<FieldType_ ClassType_::*> {
	using ClassType = ClassType_;
	using FieldType = FieldType_;
};

template<typename T> struct MemberMethodPointer {};
template<typename ClassType_, typename ReturnType_>
struct MemberMethodPointer<ReturnType_ ClassType_::*()> {
	using ClassType = ClassType_;
	using ReturnType = ReturnType_;
};

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


//ok me trying to get templates to auto-deduce
// seems like it doesn't want to deduce based on return type / assignment, 
//  so instead how about a function for when the templated class doesn't change, but the parameter does
template<
	template<typename Value> typename Container,
	typename FromElem,
	//I could have template ToElem, then replace "FunctionType f" with "std::function<ToType(FromType)> f", but I'm getting template type substitution failures with this.
	//typename ToElem
	//Wo instead, I'll match the arg and deduce the result type for the container.
	typename FunctionType	
>
Container<
	decltype(std::declval<FunctionType>()(FromElem()))//ToElem
> mapValues(
	Container<FromElem> const & from,
	FunctionType f
) {
	Container<
		decltype(std::declval<FunctionType>()(FromElem()))//ToElem
	> to;
	for (FromElem const & fromElem : from) {
		to.insert(to.end(), f(fromElem));
	}
	return to;
}

template<
	template<typename Value> typename Container,
	typename FromElem,
	//typename ToElem
	typename MemberFieldType
>
Container<
	//ToElem
	//decltype(std::declval<FromElem>().*(std::declval<MemberFieldType>()))
	typename MemberPointer<MemberFieldType>::FieldType
> mapValuesToMemberField(
	Container<FromElem> const & from,
	MemberFieldType fromElemField
	//ToElem FromElem::*fromElemField
) {
	Container<
		//ToElem
		//decltype(std::declval<FromElem>().*(std::declval<MemberFieldType>()))
		typename MemberPointer<MemberFieldType>::FieldType
	> to;
	for (FromElem const & fromElem : from) {
		to.insert(to.end(), fromElem.*fromElemField);
	}
	return to;
}

template<
	template<typename Value> typename Container,
	typename FromElem,
	//typename ToElem
	typename MemberMethodType
>
Container<
	//ToElem
	decltype((std::declval<FromElem>().*(std::declval<MemberMethodType>()))())
	//typename MemberMethodPointer<MemberMethodType>::ReturnType
> mapValuesToMemberMethod(
	Container<FromElem> const & from,
	MemberMethodType fromElemMethod
	//ToElem (FromElem::*fromElemMethod)()
) {
	Container<
		//ToElem
		decltype((std::declval<FromElem>().*(std::declval<MemberMethodType>()))())
		//typename MemberMethodPointer<MemberMethodType>::ReturnType
	> to;
	for (FromElem const & fromElem : from) {
		to.insert(to.end(), (fromElem.*fromElemMethod)());
	}
	return to;
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
