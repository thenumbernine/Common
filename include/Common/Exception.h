#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <exception>

namespace Common {

class Exception : public std::exception {
protected:
	std::ostringstream s;
	mutable std::string msg;
public:
	Exception() throw() { }
	Exception(const Exception &t) throw() { s << t.s.str(); }
	Exception &operator=(const Exception &t) throw() { s << t.s.str(); return *this; }
	virtual ~Exception() throw() {};

	virtual const char *what() const throw() { 
		msg = s.str();
		return msg.c_str();
	}

	template<typename T>
	Exception &operator<<(const T &stuff) {
		//hmm... can't specify to use global operator<< unless it is already defined...
		//using ::operator<<;
		s << stuff;
		return *this;
	}
};

inline std::ostream &operator<<(std::ostream &s, const Exception &t) {
	return s << t.what();
}

}
