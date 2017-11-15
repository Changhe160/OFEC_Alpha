#define BOOST_TEST_MODULE CEC2015_global
#include <boost/test/unit_test.hpp>

#include "../instance/problem/continuous/global/CEC2015/F5_SR_schwefel.h"
#include "../instance/problem/continuous/global/CEC2015/F8_hybrid3.h"
#include "../instance/problem/continuous/global/CEC2015/F13_global_composition5.h"
using namespace OFEC;

BOOST_AUTO_TEST_SUITE(CEC2015_global_test)


BOOST_AUTO_TEST_CASE(test_case1) {
	global::ms_global = std::unique_ptr<global>(new global(0.5, 0.5));
	size_t dim(10);
	CEC2015::F5_SR_schwefel a("F5_SR_schwefel", dim, 1);

	//std::vector<real> data = { 0, 0 };
	std::vector<real> data(dim, 0);
	for (size_t i = 0; i < a.translation().size(); ++i)
		data[i] += a.translation()[i];

	solution<variable<real>> sol(1, data);

	a.evaluate(sol, caller::Problem);

	BOOST_CHECK_EQUAL(a.get_optima().single_objective(0), a.get_optima().single_objective(0));
	std::cout << sol.get_objective()[0] << std::endl;
	//for (size_t i = 0; i < a.get_optima().number_variable(); ++i)
	//	std::cout << a.get_optima().single_objective(i) << std::endl;


}
BOOST_AUTO_TEST_CASE(test_case2) {
	global::ms_global = std::unique_ptr<global>(new global(0.5, 0.5));
	size_t dim(10);
	CEC2015::F8_hybrid3 a("F8_hybrid3", dim, 1);

	//std::vector<real> data = { 0, 0, 1, 0, 0, 1, 0, 1, 0, 0 }; // for F7_hybrid2
	std::vector<real> data = { 0, 0, 0, 0, 1, 0, 0, 1, 0, 0 }; // for F8_hybrid3
	//std::vector<real> data(dim, 0);
	
	solution<variable<real>> sol(1, data);

	a.evaluate(sol, caller::Problem);

	BOOST_CHECK_EQUAL(a.get_optima().single_objective(0), a.get_optima().single_objective(0));
	std::cout << sol.get_objective()[0] << std::endl;
	//for (size_t i = 0; i < a.get_optima().number_variable(); ++i)
	//	std::cout << a.get_optima().single_objective(i) << std::endl;

}
BOOST_AUTO_TEST_CASE(test_case3) {
	global::ms_global = std::unique_ptr<global>(new global(0.5, 0.5));
	size_t dim(10);
	CEC2015::F13_global_composition5 a("F13_global_composition5", dim, 1);

	//std::vector<real> data(dim, 0);
	std::vector<real> data = { 0, 0, 0, 0, 1, 0, 0, 1, 0, 0 }; // for F13
	for (size_t i = 0; i < data.size(); ++i) {
		data[i] += a.get_hybrid(0)->hybrid_translation()[i];  // for F13
		//data[i] += a.get_function(0)->translation()[i];
	}
	solution<variable<real>> sol(1, data);

	a.evaluate(sol, caller::Problem);

	BOOST_CHECK_EQUAL(a.get_optima().single_objective(0), a.get_optima().single_objective(0));
	std::cout << sol.get_objective()[0] << std::endl;
	//for (size_t i = 0; i < a.get_optima().number_variable(); ++i)
	//	std::cout << a.get_optima().single_objective(i) << std::endl;

}

BOOST_AUTO_TEST_SUITE_END()
