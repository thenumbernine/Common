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

// https://stackoverflow.com/a/6894436
template<std::size_t I = 0, typename FuncT, typename... Tp>
inline typename std::enable_if_t<I == sizeof...(Tp), void>
TupleForEach(const std::tuple<Tp...> &, FuncT) {}

template<std::size_t I = 0, typename FuncT, typename... Tp>
inline typename std::enable_if_t<I < sizeof...(Tp), void>
TupleForEach(const std::tuple<Tp...>& t, FuncT f)
{
	f(std::get<I>(t), I);
	TupleForEach<I + 1, FuncT, Tp...>(t, f);
}

}
