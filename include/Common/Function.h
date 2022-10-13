#pragma once

#include "Common/MemberPointer.h"	//MemberMethodPointer
#include <tuple>

namespace Common {

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

// function from lambda
// get a Function<> above for the signature of a lambda based on its operator() member method
template<typename Lambda>
using FunctionFromLambda = Function<typename MemberMethodPointer<decltype(&Lambda::operator())>::CFunc>;

}
