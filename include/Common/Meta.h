#pragma once

#include "Common/MemberPointer.h"	//MemberMethodPointer
#include "Common/Tuple.h"	//tuple_rep_t
#include <functional>	//function
#include <tuple>		//tuple
#include <algorithm>	//transform
#include <iterator>		//inserter

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

// TODO how about just a template<typename,typename> apply_all ?
// then is_all_base_of_v<T,Ts...> = apply_all<std::is_base_of, T, Ts...>;
template<typename T, typename... Us>
struct is_all_base_of;
template<typename T, typename U, typename... Us>
struct is_all_base_of<T,U,Us...> {
	static constexpr bool value = std::is_base_of_v<T,U>
		&& is_all_base_of<T, Us...>::value;
};
template<typename T>
struct is_all_base_of<T> {
	static constexpr bool value = true;
};
template<typename T, typename... Us>
concept is_all_base_of_v = is_all_base_of<T, Us...>::value;

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
	using DestT = std::invoke_result_t<Transform,ContainerT>;
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
	typename Container,
	typename MemberMethod
>
auto mapValuesToMemberMethod(
	Container const & from,
	MemberMethod fromElemMethod
) {
	return mapValues(from, [fromElemMethod](
		auto const & fromElem
	) {
		return (fromElem.*fromElemMethod)();
	});
}

}
