#pragma once

#include <functional>

//common metaprograms

/*
vector of types 
TypeVector<int, char, ...>::Get<1> is type char
TypeVector<int, char, double>::size == 3
*/

template<int index, typename... Args>
struct GetTypeVector;

template<int index, typename Arg, typename... Args> 
struct GetTypeVector<index, Arg, Args...> {
	typedef typename std::conditional<
		index == 0,
		Arg,
		typename GetTypeVector<index-1, Args...>::Type
	>::type Type;
};

template<typename Arg, typename... Args>
struct GetTypeVector<0, Arg, Args...> {
	typedef Arg Type;
};

template<typename Arg>
struct GetTypeVector<0, Arg> {
	typedef Arg Type;
};

template<typename... Args>
struct TypeVector;

template<>
struct TypeVector<> {
	enum { size = 0 };
	template<int index>
	using Get = void;
};

template<typename Arg, typename... Args>
struct TypeVector<Arg, Args...> {
	typedef TypeVector<Args...> Next;
	enum { size = Next::size + 1 };

	template<int index>
	using Get = typename GetTypeVector<index, Arg, Args...>::Type;
};

//type concat

template<typename Arg1, typename Arg2>
struct ConcatTypeVector;

template<typename... Arg1, typename... Arg2>
struct ConcatTypeVector<TypeVector<Arg1...>, TypeVector<Arg2...>> {
	typedef TypeVector<Arg1..., Arg2...> Type;
};

//function parameters

template<typename Return, typename... ArgList>
struct Function;

template<typename Return_, typename... ArgList>
struct Function<Return_(ArgList...)> {
	typedef Return_ Type(ArgList...);
	typedef Return_ Return;
	typedef TypeVector<ArgList...> Args;
	enum { numArgs = Args::size };
	
	template<int index>
	using Arg = typename Args::template Get<index>;
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

