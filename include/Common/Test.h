#pragma once

//macros for unit tests

#include "Common/Exception.h"

#define ECHO(x)\
	std::cout\
		<< __FILE__ << ":" << __LINE__ << ": "\
		<< #x << " :: " << (x) << std::endl;

#define TEST_BOOL(expr)\
	{\
		std::ostringstream ss;\
		ss << __FILE__ << ":" << __LINE__ << ": " << #expr << " :: " << (expr);\
		std::string msg = ss.str();\
		if (!(expr)) {\
			msg += " FAILED!";\
			std::cout << msg << std::endl;\
			throw Common::Exception() << msg;\
		}\
		std::cout << msg << std::endl;\
	}

#define TEST_OP(a,b,op)\
	{\
		std::ostringstream ss;\
		ss << __FILE__ << ":" << __LINE__ << ": " << #a << " " #op " " << #b << " :: " << (a) << " " #op " " << (b);\
		std::string msg = ss.str();\
		if (!((a) op (b))) {\
			msg += " FAILED!";\
			std::cout << msg << std::endl;\
			throw Common::Exception() << msg;\
		}\
		std::cout << msg << std::endl;\
	}

#define TEST_EQ(a,b) TEST_OP(a,b,==)
#define TEST_NE(a,b) TEST_OP(a,b,!=)
#define TEST_LT(a,b) TEST_OP(a,b,<)
#define TEST_LE(a,b) TEST_OP(a,b,<=)
#define TEST_GT(a,b) TEST_OP(a,b,>)
#define TEST_GE(a,b) TEST_OP(a,b,>=)
#define TEST_EQ_EPS(a,b,epsilon) TEST_LE(fabs(a-b), epsilon)
