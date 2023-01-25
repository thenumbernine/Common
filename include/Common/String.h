#pragma once

#include "Common/Reflection.h"	//has_fields_v
#include "Common/Meta.h"		//TupleForEach
#include <sstream>
#include <vector>


//not sure which of these I need
#include <numeric>
#include <map>
#include <regex>
#include <string>


// should all std::to_string's come first?

namespace std {
//for template argument sake this should have always been defined
inline std::string const & to_string(std::string const & s) {
	return s;
}

}

namespace Common {

template<typename T>
constexpr bool has_to_ostream_v = requires(T const & t) { &T::to_ostream; };

// add static constexpr auto dontUseFieldsOStream to prevent automatic ostream<<  use from your class with its 'fields' static tuple
// is this getting out of hand?
template<typename T>
constexpr bool has_dontUseFieldsOStream_v = requires(T const & t) { T::dontUseFieldsOStream; };

}

/*
Allow a default ostream operator<< for objects that have the member function 'to_ostream(ostream& o) const'
TODO hmm why isn't it working? failing for Tensor::vec::iterator

Wait isn't namespace operator resolve need this to be defined in the same namespace as whatever is using it?
and if that's true, then making a templated version of this function seems kinda pointless / counter-productive.

Ok now I'm asking why do this, because providing a member to_ostream is probably more rigid than just providing an overload of operator<<(ostream)
and the reason I can think of is what about templated nested classes and code organization, putting the templated nested class ostream method inside/next to the class rather than half way down the file (vec::ReadIterator)
*/
template<typename T>
requires Common::has_to_ostream_v<T>
std::ostream & operator<<(std::ostream & o, T const & t) {
	return t.to_ostream(o);
}

//default ostream operator<< for objects that have the field 'fields'...
// if we don't have a 'to_ostream' but we do have 'fields' then use that
template<typename T>
requires (
	!Common::has_to_ostream_v<T>
	&& Common::has_fields_v<T>
	&& !Common::has_dontUseFieldsOStream_v<T> 
)
std::ostream& operator<<(std::ostream& o, T const & b) {
	o << "{";
	Common::TupleForEach(T::fields, [&o, &b](auto const & x, size_t i) constexpr -> bool {
		auto const & name = std::get<0>(x);
		auto const & field = std::get<1>(x);
		if (i > 0) o << ", ";
		o << name << "=" << b.*field;
		return false;
	});
	o << "}";
	return o;
}

namespace Common {

//https://stackoverflow.com/questions/16749069/c-split-string-by-regex
#include <regex>
template<typename T>
T split(std::string const & string_to_split, std::string const & regexPattern) {
	std::regex rgx(regexPattern);
	std::sregex_token_iterator iter(string_to_split.begin(),
		string_to_split.end(),
		rgx,
		-1);
	std::sregex_token_iterator end;
	T result;
	for ( ; iter != end; ++iter) {
		result.push_back(*iter);
	}
	return result;
}

template<typename T>
std::string concat(T const & v, std::string const & sep) {
	bool first = true;
	std::string result = "";
	for (auto const & s : v) {
		if (!first) result += sep;
		result += std::to_string(s);
		first = false;
	}
	return result;
}

//out of place, maybe not used
#include <numeric>
template<typename T>
typename T::value_type sum(T const & t) {
	return std::accumulate(t.begin(), t.end(), typename T::value_type());
}

//produce a string based on an object's ostream operator<<
template<typename T>
std::string objectStringFromOStream(T const & x) {
	//using ::operator<<; // seeing this always seems like a bad sign
	std::ostringstream ss;
	ss << x;
	return ss.str();
}

template<typename T>
std::ostream & iteratorToOStream(std::ostream & o, T const & v) {
	o << "{";
	char const * sep = "";
	for (auto const & x : v) {
		o << sep << x;
		sep = ", ";
	}
	return o << "}";
}

} // namespace Common

namespace std {

//default vector ostream operator<<
template<typename T>
std::ostream & operator<<(std::ostream& o, std::vector<T> const & v) {
	return Common::iteratorToOStream<decltype(v)>(o, v);
}

//default array ostream operator<<
template<typename T, size_t N>
std::ostream & operator<<(std::ostream& o, std::array<T,N> const & v) {
	return Common::iteratorToOStream<decltype(v)>(o, v);
}

template<typename T>
std::string to_string(std::vector<T> const & x) {
	return Common::objectStringFromOStream(x);
}

//if a typename has 'fields' then std::to_string will use objectStringFromOStream
template<typename T>
requires (
	Common::has_fields_v<T>
	&& !Common::has_dontUseFieldsOStream_v<T> 
)
std::string to_string(T const & x) {
	return Common::objectStringFromOStream(x);
}

} // namespace std
