#define BOOST_TEST_MODULE CEC2013_MMO
#include <boost/test/unit_test.hpp>

#include "../instance/problem/continuous/multi_modal/CEC2013/F1_F8.h"
#include "../instance/problem/continuous/multi_modal/CEC2013/F12_composition4.h"

using namespace OFEC;

BOOST_AUTO_TEST_SUITE(CEC2013_MMO_test)


BOOST_AUTO_TEST_CASE(test_case1) {
	global::ms_global = std::unique_ptr<global>(new global(0.5, 0.5));

	CEC2013::F8_modified_rastrigin a("F8_modified_rastrigin", 2, 1);

	std::vector<real> data = { 0, 0 }; 
	std::vector<real> data2 = { -1, 1 };

	solution<variable<real>> sol(1, data);
	solution<variable<real>> sol2(1, data2);


	//sphere a("", data.size(), 1);


	a.evaluate(sol, caller::Problem);
//	a.evaluate(sol2, caller::Problem);
	BOOST_CHECK_EQUAL(a.get_optima().single_objective(0), sol.get_objective()[0]);
	//BOOST_CHECK_EQUAL(a.get_optima().single_objective(0), sol2.get_objective()[0]);
	for (size_t i = 0; i < a.get_optima().number_variable();++i)
		std::cout << a.get_optima().single_objective(i) << std::endl;

}
BOOST_AUTO_TEST_CASE(test_case2) {
	global::ms_global = std::unique_ptr<global>(new global(0.5, 0.5));

	CEC2013::F12_composition4 a("F12_composition_F4", 2, 1);

	std::vector<real> data = { 0, 0 };
	//std::vector<real> data2 = { -1, 1 };


	for (size_t i = 0; i < data.size(); ++i) {
		data[i] += a.get_function(0)->translation()[i];
	}
	solution<variable<real>> sol(1, data);
	//solution<variable<real>> sol2(1, data2);


	//sphere a("", data.size(), 1);


	a.evaluate(sol, caller::Problem);
	//a.evaluate(sol2, caller::Problem);
	BOOST_CHECK_EQUAL(a.get_optima().single_objective(0), sol.get_objective()[0]);
	//BOOST_CHECK_EQUAL(a.get_optima().single_objective(0), sol2.get_objective()[0]);

	for (size_t i = 0; i < a.get_optima().number_variable(); ++i)
		std::cout << a.get_optima().single_objective(i) << std::endl;
}


BOOST_AUTO_TEST_SUITE_END()