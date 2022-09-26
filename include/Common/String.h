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
#include <sstream>


//default ostream operator<< for objects that have the field 'fields'...
template<typename T> requires Common::has_fields_v<T>
inline std::ostream& operator<<(std::ostream& o, T const & b) {
	o << "[";
	Common::TupleForEach(T::fields, [&o, &b](auto const & x, size_t i) constexpr -> bool {
		auto const & name = std::get<0>(x);
		auto const & field = std::get<1>(x);
		if (i > 0) o << ", ";
		o << name << "=" << b.*field;
		return false;
	});
	o << "]";
	return o;
}

namespace Common {

//produce a string based on an object's ostream operator<<
template<typename T>
std::string objectStringFromOStream(T const & x) {
	std::ostringstream ss;
	ss << x;
	return ss.str();
}

}

//default vector ostream operator<<
template<typename T>
std::ostream& operator<<(std::ostream& o, std::vector<T> const & v) {
	o << "[";
	char const * sep = "";
	for (auto const & x : v) {
		o << sep << x;
		sep = ", ";
	}
	return o << "]";
}

namespace std {

//if a typename has 'fields' then std::to_string will use objectStringFromOStream
template<typename T> requires Common::has_fields_v<T>
inline std::string to_string(T const & x) {
	return Common::objectStringFromOStream(x);
}

//for template argument sake this should have always been defined
inline std::string const & to_string(std::string const & s) {
	return s;
}

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

}
