#include "Common/Test.h"
#include "Common/Meta.h"
#include "Common/Variadic.h"
#include "Common/Sequence.h"
#include "Common/Function.h"
#include "Common/ForLoop.h"
#include <iostream>
#include <type_traits>
#include <map>
#include <vector>

namespace Test {
	using namespace Common;
	using namespace std;

	STATIC_ASSERT_EQ((tuple_find_v<char, tuple<>>), -1);
	STATIC_ASSERT_EQ((tuple_find_v<char, tuple<char>>), 0);
	STATIC_ASSERT_EQ((tuple_find_v<char, tuple<float>>), -1);
	STATIC_ASSERT_EQ((tuple_find_v<char, tuple<char, float>>), 0);
	STATIC_ASSERT_EQ((tuple_find_v<char, tuple<float, char>>), 1);

	using yesno = tuple_get_filtered_indexes_t<
		tuple<float, int, double, char>,
		is_integral
	>;
	static_assert(is_same_v<typename yesno::has, integer_sequence<int, 1, 3>>);
	static_assert(is_same_v<typename yesno::hasnot, integer_sequence<int, 0, 2>>);



	static_assert(variadic_get_v<0,size_t,1> == 1);
	static_assert(variadic_get_v<0,size_t,1,2> == 1);
	static_assert(variadic_get_v<1,size_t,1,2> == 2);
	
	static_assert(variadic_get_v<0,int,1> == 1);
	static_assert(variadic_get_v<0,int,1,2> == 1);
	static_assert(variadic_get_v<1,int,1,2> == 2);

	STATIC_ASSERT_EQ((seq_get_v<0, integer_sequence<size_t,7>>), 7);
	STATIC_ASSERT_EQ((seq_get_v<0, integer_sequence<int,7>>), 7);

	static_assert(
		is_same_v<
			seq_cat_t<int>,
			integer_sequence<int>
		>
	);
	static_assert(
		is_same_v<
			seq_cat_t<int, integer_sequence<int, 3,2>>,
			integer_sequence<int, 3,2>
		>
	);
	static_assert(
		is_same_v<
			seq_cat_t<
				int,
				integer_sequence<int, 1>,
				integer_sequence<int, 2,3>
			>,
			integer_sequence<int, 1,2,3>
		>
	);
	static_assert(
		is_same_v<
			seq_cat_t<
				int,
				integer_sequence<int, 1>,
				integer_sequence<int, 2,3>,
				integer_sequence<int, 4,5>
			>,
			integer_sequence<int, 1,2,3,4,5>
		>
	);

	static_assert(is_same_v<seq_set_t<index_sequence<2>, 0, 3>, index_sequence<3>>);
	static_assert(is_same_v<seq_set_t<index_sequence<1, 2>, 0, 3>, index_sequence<3, 2>>);
	static_assert(is_same_v<seq_set_t<index_sequence<1, 2>, 1, 3>, index_sequence<1, 3>>);
	static_assert(is_same_v<seq_set_t<index_sequence<7, 5, 9>, 0, 3>, index_sequence<3, 5, 9>>);
	static_assert(is_same_v<seq_set_t<index_sequence<7, 5, 9>, 1, 3>, index_sequence<7, 3, 9>>);
	static_assert(is_same_v<seq_set_t<index_sequence<7, 5, 9>, 2, 3>, index_sequence<7, 5, 3>>);

	static_assert(constexpr_min(1,2) == 1);
	static_assert(constexpr_min(4,3) == 3);

	static_assert(variadic_min_v<size_t,7> == 7);
	static_assert(variadic_min_v<size_t,3,8> == 3);
	static_assert(variadic_min_v<int,9,1> == 1);
	static_assert(variadic_min_v<int,2,4,8> == 2);
	static_assert(variadic_min_v<short,5,3,8> == 3);
	static_assert(variadic_min_v<short,9,5,1> == 1);

	static_assert(seq_min_v<index_sequence<7>> == 7);
	static_assert(seq_min_v<index_sequence<3,8>> == 3);
	static_assert(seq_min_v<integer_sequence<int,9,1>> == 1);
	static_assert(seq_min_v<integer_sequence<int,2,4,8>> == 2);
	static_assert(seq_min_v<integer_sequence<short,5,3,8>> == 3);
	static_assert(seq_min_v<integer_sequence<short,9,5,1>> == 1);

	//static_assert(variadic_min_loc_v<size_t,> == 0); // should error
	static_assert(variadic_min_loc_v<size_t,7> == 0);
	static_assert(variadic_min_loc_v<size_t,3,8> == 0);
	static_assert(variadic_min_loc_v<size_t,9,1> == 1);
	static_assert(variadic_min_loc_v<size_t,2,4,8> == 0);
	static_assert(variadic_min_loc_v<size_t,5,3,8> == 1);
	static_assert(variadic_min_loc_v<size_t,9,5,1> == 2);

	static_assert(seq_min_loc_v<index_sequence<3>> == 0);
	static_assert(seq_get_v<seq_min_loc_v<index_sequence<3>>, index_sequence<3>> == 3);
	static_assert(is_same_v<seq_sort_t<index_sequence<1>>, index_sequence<1>>);
	static_assert(is_same_v<seq_sort_t<index_sequence<3,4>>, index_sequence<3,4>>);
	static_assert(is_same_v<seq_sort_t<index_sequence<4,3>>, index_sequence<3,4>>);
	static_assert(is_same_v<seq_sort_t<index_sequence<2,5,8>>, index_sequence<2,5,8>>);
	static_assert(is_same_v<seq_sort_t<index_sequence<2,8,5>>, index_sequence<2,5,8>>);
	static_assert(is_same_v<seq_sort_t<index_sequence<5,2,8>>, index_sequence<2,5,8>>);
	static_assert(is_same_v<seq_sort_t<index_sequence<5,8,2>>, index_sequence<2,5,8>>);
	static_assert(is_same_v<seq_sort_t<index_sequence<8,2,5>>, index_sequence<2,5,8>>);
	static_assert(is_same_v<seq_sort_t<index_sequence<8,5,2>>, index_sequence<2,5,8>>);

	static_assert(is_same_v<
		index_sequence<2>,
		seq_reverse_t<index_sequence<2>>
	>);
	static_assert(is_same_v<
		index_sequence<2,4>,
		seq_reverse_t<index_sequence<4,2>>
	>);
	static_assert(is_same_v<
		index_sequence<9,4,5>,
		seq_reverse_t<index_sequence<5,4,9>>
	>);

	static_assert(is_same_v<tuple_subset_t<tuple<int, char*, double>, 0, 0>,tuple<>>);
	static_assert(is_same_v<tuple_subset_t<tuple<int, char*, double>, 0, 1>,tuple<int>>);
	static_assert(is_same_v<tuple_subset_t<tuple<int, char*, double>, 0, 2>,tuple<int, char*>>);
	static_assert(is_same_v<tuple_subset_t<tuple<int, char*, double>, 0, 3>,tuple<int, char*, double>>);
	static_assert(is_same_v<tuple_subset_t<tuple<int, char*, double>, 1, 1>,tuple<>>);
	static_assert(is_same_v<tuple_subset_t<tuple<int, char*, double>, 1, 2>,tuple<char*>>);
	static_assert(is_same_v<tuple_subset_t<tuple<int, char*, double>, 1, 3>,tuple<char*, double>>);
	static_assert(is_same_v<tuple_subset_t<tuple<int, char*, double>, 2, 2>,tuple<>>);
	static_assert(is_same_v<tuple_subset_t<tuple<int, char*, double>, 2, 3>,tuple<double>>);
	static_assert(is_same_v<tuple_subset_t<tuple<int, char*, double>, 3, 3>,tuple<>>);

	static_assert(is_same_v<
		tuple_cat_t<
			tuple<>,
			tuple<>
		>,
		tuple<>
	>);
	static_assert(is_same_v<
		tuple_cat_t<
			tuple<double>,
			tuple<>
		>,
		tuple<double>
	>);
	static_assert(is_same_v<
		tuple_cat_t<
			tuple<>,
			tuple<double>
		>,
		tuple<double>
	>);
	static_assert(is_same_v<
		tuple_cat_t<
			tuple<double>,
			tuple<>,
			tuple<>
		>,
		tuple<double>
	>);
	static_assert(is_same_v<
		tuple_cat_t<
			tuple<>,
			tuple<double>,
			tuple<>
		>,
		tuple<double>
	>);
	static_assert(is_same_v<
		tuple_cat_t<
			tuple<>,
			tuple<>,
			tuple<double>
		>,
		tuple<double>
	>);

	static_assert(is_same_v<
		make_index_range<0, 4>,
		index_sequence<0, 1, 2, 3>
	>);
	static_assert(is_same_v<
		make_index_range<4, 0>,
		index_sequence<4, 3, 2, 1>
	>);
	static_assert(is_same_v<
		make_index_range<4, 4>,
		index_sequence<>
	>);
	static_assert(is_same_v<
		make_index_range<0, 0>,
		index_sequence<>
	>);

	static_assert(is_same_v<tuple_subset_t<tuple<>, 0, 0>,tuple<>>);
	static_assert(is_same_v<tuple_subset_t<tuple<>, 0, 0>,tuple<>>);

	static_assert(is_same_v<
		tuple_insert_t<tuple<>, 0, tuple<>>,
		tuple<>
	>);

	static_assert(is_same_v<
		tuple_insert_t<
			tuple<int, char*, double>,
			0,
			tuple<short, long>
		>,
		tuple<short, long, int, char*, double>
	>);
	static_assert(is_same_v<
		tuple_insert_t<
			tuple<int, char*, double>,
			1,
			tuple<short, long>
		>,
		tuple<int, short, long, char*, double>
	>);
	static_assert(is_same_v<
		tuple_insert_t<
			tuple<int, char*, double>,
			2,
			tuple<short, long>
		>,
		tuple<int, char*, short, long, double>
	>);
	static_assert(is_same_v<
		tuple_insert_t<
			tuple<int, char*, double>,
			3,
			tuple<short, long>
		>,
		tuple<int, char*, double, short, long>
	>);

}



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

template<int i> 
struct ForSeqTest {
	static bool exec() {
		std::cout << "for_seq " << i << std::endl;
		return false;
	}
};

void testForLoop() {
	//template-based for-loop
	
	Common::ForLoop<0, 2, TestEq>::exec("hello");
	
	//test Common::Function too , make sure it deduces the 'bool(string)' signature
	static_assert((std::is_same_v<
		Common::Function<decltype(TestEq<0>::exec)>,
		Common::FunctionFromTupleArgs<bool, Common::tuple_rep_t<std::string const &, 1>>
	>));
	
	Common::ForLoop<0, 1, TestEq>::exec("hello");
	
	Common::ForLoop<0, 0, TestEq>::exec("hello");
	
	Common::ForLoop<1, 0, TestEq>::exec("hello");
	Common::ForLoop<1, -1, TestEq>::exec("hello");

	// constexpr-based for-loop over tuples

	Common::TupleForEach(std::make_tuple("a", 2.5, 'c'), [](auto x, size_t i) constexpr -> bool {
		std::cout << "TupleForEach " << i << "'th is " << x << std::endl;
		return false; // false == continue, true == break
	});
	
	using S = std::index_sequence<1,2,3>;
	Common::for_seq_runtime<S>([](size_t i) constexpr -> bool {
		std::cout << "for_seq_runtime " << i << "'th" << std::endl;
		return false;
	});

	Common::for_seq<std::make_index_sequence<4>, ForSeqTest>();
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
