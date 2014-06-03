#pragma once

#include <functional>

//common metaprograms

//for loop

template<int index, int end, typename Op>
struct ForLoop {
	//compile-time for-loop callback function
	//return 'true' to break the loop
	//with loop unrolling already built into compilers, 
	// this for-loop metaprogram probably is slower than an ordinary for-loop 
	static bool exec(typename Op::Input &input) {
		typedef typename Op::template Exec<index> Exec;
		if (Exec::exec(input)) return true;
		return ForLoop<index+1,end,Op>::exec(input);
	}
};

template<int end, typename Op>
struct ForLoop<end, end, Op> {
	static bool exec(typename Op::Input &input) {
		return false;
	}
};

//if condition

template<bool cond, typename A, typename B>
struct If;

template<typename A, typename B>
struct If<true, A, B> {
	typedef A Type;
};

template<typename A, typename B>
struct If<false, A, B> {
	typedef B Type;
};

/*
vector of types 
TypeVector<int, char, ...>::Get<1> is type char
TypeVector<int, char, double>::size == 3
*/

template<int index, typename... Args>
struct GetTypeVector;

template<int index, typename Arg, typename... Args> 
struct GetTypeVector<index, Arg, Args...> {
	typedef typename If<
		index == 0,
		Arg,
		typename GetTypeVector<index-1, Args...>::Type
	>::Type Type;
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

//function parameters

template<typename Return, typename... ArgList>
struct FunctionInfo;

template<typename Return_, typename... ArgList>
struct FunctionInfo<Return_(ArgList...)> {
	typedef Return_ Return;
	typedef TypeVector<ArgList...> Args;
	enum { numArgs = Args::size };
	
	template<int index>
	using Arg = typename Args::template Get<index>;
};

