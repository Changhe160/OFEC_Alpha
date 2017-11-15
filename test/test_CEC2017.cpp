/*#define BOOST_TEST_MODULE CEC2017
#include <boost/test/unit_test.hpp>

#include "../instance/problem/continuous/constrained/CEC2017/C19.h"    
#include "../instance/problem/continuous/constrained/CEC2017/C23.h"
using namespace OFEC;

BOOST_AUTO_TEST_SUITE(CEC2017_test)


BOOST_AUTO_TEST_CASE(test_case1) {
	global::ms_global = std::unique_ptr<global>(new global(0.5, 0.5));

	CEC2017::C19 a("C19", 10, 1);

	//vector<real> data = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
	std::vector<real> data(10, 0);

	
	solution<variable<real>> sol(1, data);

	a.evaluate(sol, caller::Problem,false);
	//BOOST_CHECK_EQUAL(a.get_optima().single_objective(0), sol.get_objective()[0]);

	std::cout << sol.get_objective()[0] << std::endl;
	
	std::cout << sol.constraint_value().first << std::endl;
	for (auto &i : sol.constraint_value().second)
		std::cout << i << std::endl;
	std::cout << "number of violations is: " << sol.num_violation() << std::endl;
	//std::cout << sol.constraint_value() << std::endl;
}
BOOST_AUTO_TEST_CASE(test_case2) {
	global::ms_global = std::unique_ptr<global>(new global(0.5, 0.5));

	CEC2017::C23 a("C23", 10, 1);

	//vector<real> data = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
	std::vector<real> data(a.translation());
	
	
	solution<variable<real>> sol(1, data);

	a.evaluate(sol, caller::Problem, false);
	//BOOST_CHECK_EQUAL(a.get_optima().single_objective(0), sol.get_objective()[0]);

	std::cout << sol.get_objective()[0] << std::endl;

	std::cout << sol.constraint_value().first << std::endl;
	for(auto &i: sol.constraint_value().second)
		std::cout << i << std::endl;
	std::cout << "number of violations is: " << sol.num_violation() << std::endl;
}


BOOST_AUTO_TEST_SUITE_END()*/