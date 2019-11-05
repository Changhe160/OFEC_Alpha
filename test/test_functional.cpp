#include "../utility/catch.hpp"
#include "../utility/functional.h"
#include "../core/global.h"
#include "../instance/problem/continuous/multi_objective/DTLZ/DTLZ1.h"
#include <iostream>

using namespace OFEC;

TEST_CASE("domination relationship", "[domination]")
{
	global::ms_global = std::unique_ptr<global>(new global(1, 0.5, 0.5));
	global::ms_global->m_problem.reset(new DTLZ1("MOP_DTLZ1", 5, 3));
	global::ms_global->m_problem->initialize();

	solution<> s1(global::ms_global->m_problem->objective_size(), 0, global::ms_global->m_problem->variable_size());
	s1.initialize();
	solution<> s2(s1);

	REQUIRE(s1.equal(s2) == true);

}

TEST_CASE("merge sort", "[merge_sort]") {
	uniform rand(0.5);

	{
		std::vector<int> int_1d_array;
		size_t num = 10;
		for (size_t i = 0; i < num; i++)
			int_1d_array.push_back(rand.next_non_standard<int>(0, 10));
		std::vector<int> index;
		merge_sort(int_1d_array, num, index);
		for (auto idx : index)
			std::cout << int_1d_array[idx] << " ";
		std::cout << std::endl;
	}

	{
		std::vector<real> double_1d_array;
		size_t num = 10;
		for (size_t i = 0; i < num; i++)
			double_1d_array.push_back(rand.next_non_standard<real>(0, 10));
		std::vector<int> index;
		merge_sort(double_1d_array, num, index);
		for (auto idx : index)
			std::cout << double_1d_array[idx] << " ";
		std::cout << std::endl;
	}

	{
		std::vector<std::string> string_1d_array{ "pen","pineapple","apple","pen" };
		size_t num = string_1d_array.size();
		std::vector<int> index;
		merge_sort(string_1d_array, num, index);
		for (auto idx : index)
			std::cout << string_1d_array[idx] << " ";
		std::cout << std::endl;
	}
}