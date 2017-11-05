#define BOOST_TEST_MODULE problemtest
#include <boost/test/unit_test.hpp>


#include "../core/problem/problem.h"
#include "../core/algorithm/solution.h"

#include "../core/global.h"
#include "../instance/problem/continuous/global/BBOB/BBOB.h"

using namespace OFEC;

BOOST_AUTO_TEST_SUITE(problem_test)


BOOST_AUTO_TEST_CASE(test_case3)
{
	std::vector<double> a = { 1,1 }, b = { 0,0 };
	
	problem::allocate_memory<solution<>>(2, 2);
}

BOOST_AUTO_TEST_CASE(test_case4)
{

	RIGIESTER(problem, BBOB, "Sphere_F01", std::set<problem_tag>({ problem_tag::CONT, problem_tag::SOP }))

	global::ms_global->m_problem.reset(factory<problem>::produce("Sphere_F01",global::ms_arg));

}

BOOST_AUTO_TEST_SUITE_END()

