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

namespace Common {

template<typename T>
constexpr bool has_to_ostream_v = requires(T const & t) { &T::to_ostream; };

}

//default ostream operator<< for objects that have the member function 'to_ostream(ostream& o) const'
// TODO hmm why isn't it working? failing for Tensor::_vec::iterator
template<typename T>
requires Common::has_to_ostream_v<T>
std::ostream & operator<<(std::ostream & o, T const & t) {
	return t.to_ostream(o);
}

//default ostream operator<< for objects that have the field 'fields'...
template<typename T>
requires Common::has_fields_v<T>
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
	using ::operator<<; // seeing this always seems like a bad sign
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

}

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

namespace std {

template<typename T>
std::string to_string(std::vector<T> const & x) {
	return Common::objectStringFromOStream(x);
}

//if a typename has 'fields' then std::to_string will use objectStringFromOStream
template<typename T> requires Common::has_fields_v<T>
std::string to_string(T const & x) {
	return Common::objectStringFromOStream(x);
}

//for template argument sake this should have always been defined
inline std::string const & to_string(std::string const & s) {
	return s;
}

}


