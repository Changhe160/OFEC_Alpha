#define BOOST_TEST_MODULE problemtest
#include <boost/test/unit_test.hpp>


#include "../core/problem/problem.h"
#include "../core/algorithm/solution.h"

using namespace OFEC;

BOOST_AUTO_TEST_SUITE(problem_test)


BOOST_AUTO_TEST_CASE(test_case3)
{
	std::vector<double> a = { 1,1 }, b = { 0,0 };
	
	problem::allocate_memory<solution<>>(2, 2);
}


BOOST_AUTO_TEST_SUITE_END()

