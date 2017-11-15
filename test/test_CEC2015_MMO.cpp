/*#define BOOST_TEST_MODULE CEC2015_MMO
#include <boost/test/unit_test.hpp>

#include "../instance/problem/continuous/multi_modal/CEC2015/F13_composition2015_C5.h"
#include "../instance/problem/continuous/multi_modal/CEC2015/F15_composition2015_C7.h"

using namespace OFEC;

BOOST_AUTO_TEST_SUITE(CEC2015_MMO_test)


BOOST_AUTO_TEST_CASE(test_case1) {
	global::ms_global = std::unique_ptr<global>(new global(0.5, 0.5));
	size_t dim(10);
	CEC2015::F13_composition2015_C5 a("F13_composition2015_C5", dim, 1);

	//std::vector<real> data = { 0, 0 };
	std::vector<real> data(dim,10);
	//for (size_t i = 0; i < a.translation().size(); ++i)
		//data[i] += a.translation()[i];
		
	solution<variable<real>> sol(1, data);

	a.evaluate(sol, caller::Problem);
	
	//BOOST_CHECK_EQUAL(a.get_optima().single_objective(2), a.get_optima().single_objective(15));
	//std::cout << sol.get_objective()[0] << std::endl;
	//for (size_t i = 0; i < a.get_optima().number_variable(); ++i)
	//	std::cout << a.get_optima().single_objective(i) << std::endl;
	
	for (size_t i = 0; i < a.get_optima().number_variable();++i)
		std::cout << a.get_optima().single_objective(i) << std::endl;
	std::cout << "Number of optimum solution: " << a.get_optima().number_variable() << std::endl;
}
BOOST_AUTO_TEST_CASE(test_case2) {
	global::ms_global = std::unique_ptr<global>(new global(0.5, 0.5));
	size_t dim(10);
	CEC2015::F15_composition2015_C7 a("F15_composition2015_C7", dim, 1);

	//std::vector<real> data = { 0, 0 };
	std::vector<real> data(dim, 10);

	solution<variable<real>> sol(1, data);

	a.evaluate(sol, caller::Problem);

	std::cout << sol.get_objective()[0] << std::endl;
	
}


BOOST_AUTO_TEST_SUITE_END()*/