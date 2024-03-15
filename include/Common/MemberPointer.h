#pragma once

#include <tuple>

namespace Common {

//https://stackoverflow.com/a/43211852
template<typename T>
requires (std::is_member_object_pointer_v<T>)
struct MemberPointer {};

template<typename Class_, typename FieldType_>
// is it just me, or does sometimes member methods get matched to this?
// so I'll put this requires()
// I'm sure / I hope the STL has some mechanism for this by now, and I just need to find it and replace my stuff...
requires (std::is_member_object_pointer_v<FieldType_ Class_::*>)
struct MemberPointer<FieldType_ Class_::*> {
	using Class = Class_;
	using FieldType = FieldType_;
};

// member method pointer, but only for templated return and void() args
// TODO vararg handle all args

template<typename T>
requires (std::is_member_function_pointer_v<T>)
struct MemberMethodPointer {};

template<typename Class_, typename Return_, typename... Args_>
requires (std::is_member_function_pointer_v<Return_ (Class_::*)(Args_...)>)
struct MemberMethodPointer<Return_ (Class_::*)(Args_...)> {
	static constexpr bool is_const = false;
	static constexpr bool is_noexcept = false;
	using Class = Class_;
	using Return = Return_;
	using Args = std::tuple<Args_...>;
	//"FuncType" is what I'm using in Function above for the std::function<> wrapper type
	// in Function above I'm just calling "Type" the C function type, but here Type should be the member method type ...
	// so I'll call this "CFunc"
	using CFunc = Return_(Args_...);
};

template<typename Class_, typename Return_, typename... Args_>
requires (std::is_member_function_pointer_v<Return_ (Class_::*)(Args_...)>)
struct MemberMethodPointer<Return_ (Class_::*)(Args_...) noexcept> {
	static constexpr bool is_const = false;
	static constexpr bool is_noexcept = true;
	using Class = Class_;
	using Return = Return_;
	using Args = std::tuple<Args_...>;
	using CFunc = Return_(Args_...);
};

template<typename Class_, typename Return_, typename... Args_>
requires (std::is_member_function_pointer_v<Return_ (Class_::*)(Args_...)>)
struct MemberMethodPointer<Return_ (Class_::*)(Args_...) const> {
	static constexpr bool is_const = true;
	static constexpr bool is_noexcept = false;
	using Class = Class_;
	using Return = Return_;
	using Args = std::tuple<Args_...>;
	using CFunc = Return_(Args_...);
};

template<typename Class_, typename Return_, typename... Args_>
requires (std::is_member_function_pointer_v<Return_ (Class_::*)(Args_...)>)
struct MemberMethodPointer<Return_ (Class_::*)(Args_...) const noexcept> {
	static constexpr bool is_const = true;
	static constexpr bool is_noexcept = true;
	using Class = Class_;
	using Return = Return_;
	using Args = std::tuple<Args_...>;
	using CFunc = Return_(Args_...);
};

}
