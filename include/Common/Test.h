#pragma once

//macros for unit tests

#include "Common/Exception.h"

#define COMMA	,
#define ECHO(x)	std::cout << #x << "\t" << (x) << std::endl;
#define TEST_EQ(a,b) std::cout << #a << " == " << #b << " :: " << (a) << " == " << (b) << std::endl; if ((a) != (b)) throw Common::Exception() << "failed";
