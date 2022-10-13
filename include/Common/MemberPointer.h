#pragma once

namespace Common {

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

}
