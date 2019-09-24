#include "Common/Test.h"
#include "Common/Meta.h"
#include <iostream>
#include <type_traits>

//for-loop

template<int index>
struct TestEq {
	static bool exec(int i) { 
		using A = Common::Function<void(int, char)>::Type;
		using B = std::tuple<int, char>;
		TEST_EQ((std::is_same_v<Common::Function<A>::Arg<index>, std::tuple_element_t<index, B>>), 1);
		return false;
	}
};

void testForLoop() {
	Common::ForLoop<0, 2, TestEq>::exec(1);
	TEST_EQ((std::is_same_v<Common::Function<decltype(TestEq<0>::exec)>::Arg<0>, int>), 1);
}

int test(double, char) { return 0; }

int main() {
	//testing Common::Function ::numArgs and ::Arg<n>
	{	
		using A = Common::Function<void()>;
		TEST_EQ(A::numArgs, 0);
		TEST_EQ((std::is_same_v<A::Return, void>), true);

		using B = Common::Function<void(int)>;
		TEST_EQ(B::numArgs, 1);
		TEST_EQ((std::is_same_v<B::Return, void>), 1);
		TEST_EQ((std::is_same_v<B::Arg<0>, int>), 1);
		
		using C = Common::Function<char(int, double)>;
		TEST_EQ(C::numArgs, 2);
		TEST_EQ((std::is_same_v<C::Return, char>), 1);
		TEST_EQ((std::is_same_v<C::Arg<0>, int>), 1);
		TEST_EQ((std::is_same_v<C::Arg<1>, double>), 1);
	}

	//testing Common::Function from inferred functions
	{
		TEST_EQ(Common::Function<decltype(test)>::numArgs, 2);
		TEST_EQ((std::is_same_v<Common::Function<decltype(test)>::Return, int>), 1);
		TEST_EQ((std::is_same_v<Common::Function<decltype(test)>::Arg<0>, double>), 1);
		TEST_EQ((std::is_same_v<Common::Function<decltype(test)>::Arg<1>, char>), 1);
	}

	testForLoop();
}
