#pragma once

#include "Common/MemberPointer.h"	//MemberMethodPointer
#include "Common/Tuple.h"	//tuple_rep_t
#include <functional>	//function<>
#include <tuple>		//tuple<>

namespace Common {

//common metaprograms
// ok just calling everything "Common" isn't going to work
// I'm going to need some folders like meta/function, meta/tuple, meta/variadic, meta/seq

template<typename T, typename... Args>
concept is_all_v =
	sizeof...(Args) > 0
	&& std::is_same_v<std::tuple<Args...>, Common::tuple_rep_t<T, sizeof...(Args)>>;

//https://stackoverflow.com/a/61040973
// TODO this doesn't seem to work as often as I'd like, so an easier method is just give whatever template a base class and then test is_base_v
namespace {
    template <typename, template <typename...> typename>
    struct is_instance_impl : public std::false_type {};

    template <template <typename...> typename U, typename...Ts>
    struct is_instance_impl<U<Ts...>, U> : public std::true_type {};
}
template <typename T, template <typename ...> typename U>
using is_instance = is_instance_impl<std::decay_t<T>, U>;
template <typename T, template <typename ...> typename U>
constexpr bool is_instance_v = is_instance<T, U>::value;

// I was using optional<> to capture types without instanciating them
// but optional can't wrap references, so ...
// TODO might move this somewhere like Common/Meta.h
template<typename T>
struct TypeWrapper {
	using type = T;
};

//https://codereview.stackexchange.com/a/208195
template<typename U>
struct constness_of { 
	template <typename T>
	using apply_to_t = typename std::conditional_t<
		std::is_const_v<U>,
		typename std::add_const_t<T>,
		typename std::remove_const_t<T>
	>;
};

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

}
