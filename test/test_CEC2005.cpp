#define BOOST_TEST_MODULE CEC2005
#include <boost/test/unit_test.hpp>

#include "../instance/problem/continuous/global/CEC2005/F5_schwefel_2_6_bound.h"
#include "../instance/problem/continuous/global/CEC2005/F25_rotated_hybrid_no_bound.h"

using namespace OFEC;

BOOST_AUTO_TEST_SUITE(CEC2005_test)


BOOST_AUTO_TEST_CASE(test_case1) {
	global::ms_global = unique_ptr<global>(new global(0.5, 0.5));

	CEC2005::F5_schwefel_2_6_bound a("F5_schwefel_2_6_bound", 2, 1);

	vector<real> data = { 0, 0 };
	vector<real> data2 = { -1, 1 };

	for (size_t i = 0; i < data.size(); ++i) {
		data[i] += a.translation()[i];
	}
	
	solution<variable<real>> sol(1, data);
	solution<variable<real>> sol2(1, data2);


	//sphere a("", data.size(), 1);


	a.evaluate(sol, caller::Problem);
	a.evaluate(sol2, caller::Problem);
	BOOST_CHECK_EQUAL(a.get_optima().single_objective(0), sol.get_objective()[0]);
	//BOOST_CHECK_EQUAL(a.get_optima().single_objective(0), sol2.get_objective()[0]);

	std::cout << a.get_optima().single_objective(0) << std::endl;

}
BOOST_AUTO_TEST_CASE(test_case2) {
	global::ms_global = unique_ptr<global>(new global(0.5, 0.5));

	CEC2005::F25_rotated_hybrid_no_bound a("F25_rotated_hybrid_no_bound", 2, 1);

	vector<real> data = { 0, 0 };
	vector<real> data2 = { -1, 1 };

	
	for (size_t i = 0; i < data.size(); ++i) {
		data[i] += a.get_function(0)->translation()[i];
	}
	solution<variable<real>> sol(1, data);
	solution<variable<real>> sol2(1, data2);


	//sphere a("", data.size(), 1);


	a.evaluate(sol, caller::Problem);
	a.evaluate(sol2, caller::Problem);
	BOOST_CHECK_EQUAL(a.get_optima().single_objective(0), sol.get_objective()[0]);
	//BOOST_CHECK_EQUAL(a.get_optima().single_objective(0), sol2.get_objective()[0]);

	std::cout << a.get_optima().single_objective(0) << std::endl;
}


BOOST_AUTO_TEST_SUITE_END()