#include "Common/Test.h"
#include "Common/Meta.h"
#include <iostream>
#include <type_traits>
using namespace std;
int main() {
	typedef FunctionInfo<void()> A;
	TEST_EQ(A::numArgs, 0);
	TEST_EQ(std::is_same<A::Return COMMA void>::value, true);

	typedef FunctionInfo<void(int)> B;
	TEST_EQ(B::numArgs, 1);
	TEST_EQ(std::is_same<B::Return COMMA void>::value, 1);
	TEST_EQ(std::is_same<B::Arg<0> COMMA int>::value, 1);
	
	typedef FunctionInfo<char(int, double)> C;
	TEST_EQ(C::numArgs, 2);
	TEST_EQ(std::is_same<C::Return COMMA char>::value, 1);
	TEST_EQ(std::is_same<C::Arg<0> COMMA int>::value, 1);
	TEST_EQ(std::is_same<C::Arg<1> COMMA double>::value, 1);

	TEST_EQ(TypeVector<>::size, 0);
	TEST_EQ(TypeVector<int>::size, 1);
	TEST_EQ(TypeVector<int COMMA char>::size, 2);
	TEST_EQ(std::is_same<TypeVector<int COMMA char>::Get<1> COMMA char>::value, 1);
	TEST_EQ(std::is_same<TypeVector<int COMMA int>::Get<0> COMMA int>::value, 1);
	TEST_EQ(std::is_same<TypeVector<int>::Get<0> COMMA int>::value, 1);
}
