#include "Common/Test.h"
#include "Common/Meta.h"
#include <iostream>
#include <type_traits>

using namespace std;

//for-loop

struct TestEq {
	typedef Function<void(int, char)>::Type A;
	typedef TypeVector<int, char> B;
	
	template<int index>
	struct Exec {
		static bool exec(int i) { 
			TEST_EQ(std::is_same<Function<A>::Arg<index> COMMA B::Get<index>>::value, 1);
			return false;
		}
	};
};

void testForLoop() {
	ForLoop<0, 2, TestEq>::exec(1);
	TEST_EQ(std::is_same<Function<decltype(TestEq::Exec<0>::exec)>::Arg<0> COMMA int>::value, 1);
}

int test(double, char) { return 0; }

int main() {
	//testing Function ::numArgs and ::Arg<n>
	{	
		typedef Function<void()> A;
		TEST_EQ(A::numArgs, 0);
		TEST_EQ(std::is_same<A::Return COMMA void>::value, true);

		typedef Function<void(int)> B;
		TEST_EQ(B::numArgs, 1);
		TEST_EQ(std::is_same<B::Return COMMA void>::value, 1);
		TEST_EQ(std::is_same<B::Arg<0> COMMA int>::value, 1);
		
		typedef Function<char(int, double)> C;
		TEST_EQ(C::numArgs, 2);
		TEST_EQ(std::is_same<C::Return COMMA char>::value, 1);
		TEST_EQ(std::is_same<C::Arg<0> COMMA int>::value, 1);
		TEST_EQ(std::is_same<C::Arg<1> COMMA double>::value, 1);
	}

	//testing TypeVector ::size and ::Get<n>
	{
		TEST_EQ(TypeVector<>::size, 0);
		TEST_EQ(TypeVector<int>::size, 1);
		TEST_EQ(TypeVector<int COMMA char>::size, 2);
		TEST_EQ(std::is_same<TypeVector<int COMMA char>::Get<1> COMMA char>::value, 1);
		TEST_EQ(std::is_same<TypeVector<int COMMA int>::Get<0> COMMA int>::value, 1);
		TEST_EQ(std::is_same<TypeVector<int>::Get<0> COMMA int>::value, 1);
	}

	//testing Function from inferred functions
	{
		TEST_EQ(Function<decltype(test)>::numArgs, 2);
		TEST_EQ(std::is_same<Function<decltype(test)>::Return COMMA int>::value, 1);
		TEST_EQ(std::is_same<Function<decltype(test)>::Arg<0> COMMA double>::value, 1);
		TEST_EQ(std::is_same<Function<decltype(test)>::Arg<1> COMMA char>::value, 1);
	}

	testForLoop();
}
