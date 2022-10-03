#include "Common/Test.h"
#include "Common/Meta.h"
#include <iostream>
#include <type_traits>
#include <map>
#include <vector>

//for-loop

template<int index>
struct TestEq {
	// arg here just here to prove you can pass stuff into this function
	static bool exec(std::string const & randomarg) { 
		TEST_EQ(randomarg, std::string("hello"));
		using A = Common::Function<void(int, char)>::Type;
		using B = std::tuple<int, char>;
		TEST_EQ((std::is_same_v<Common::Function<A>::Arg<index>, std::tuple_element_t<index, B>>), 1);
		return false;
	}
};

void testForLoop() {
	Common::ForLoop<0, 2, TestEq>::exec("hello");
	
	//test Common::Function too , make sure it deduces the 'bool(string)' signature
	static_assert((std::is_same_v<
		Common::Function<decltype(TestEq<0>::exec)>,
		Common::FunctionFromTupleArgs<bool, Common::tuple_rep_t<std::string const &, 1>>
	>));
	
	Common::ForLoop<0, 1, TestEq>::exec("hello");
	
	Common::ForLoop<0, 0, TestEq>::exec("hello");
	
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

	//testing mapElem
	{
		auto v = std::vector<std::string>{ "a", "be", "see" };

		//doesn't know return container template, so can't auto-deduce
		auto x = Common::mapElems<
			std::vector<std::string>,
			std::vector<size_t>
		>(v, [](std::string const & s) -> size_t { return s.size(); });

		auto w = Common::mapElemsToMemberMethod<
			std::vector<std::string>,
			std::vector<size_t>
		>(v, &std::string::size);

		// but on the plus side, you can map to new containers
		auto m = Common::mapElems<
			std::vector<std::string>,
			std::map<std::string, size_t>
		>(
			v,
			[](std::string const & s) -> std::map<std::string, size_t>::value_type {
				return std::pair<std::string, size_t>(s, s.size());
			}
		);
		
		//does know return container template, so can auto-deduce
		//however (for now) container must be single-template-param
		auto y = Common::mapValues(v, [](std::string const & s) -> size_t { return s.size(); });
	
		{
			struct A {
				int x = {};
				A(int x_) : x(x_) {}
			};
			std::vector<A> i = {1,2,3};
			auto z = Common::mapValuesToMemberField(i, &A::x);
		}

		auto u = Common::mapValuesToMemberMethod(v, &std::string::size);
	}
}
