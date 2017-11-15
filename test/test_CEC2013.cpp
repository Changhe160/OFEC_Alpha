/*#define BOOST_TEST_MODULE CEC2013
#include <boost/test/unit_test.hpp>

#include "../instance/problem/continuous/large_scale/CEC2013/ShiftedElliptic_F1.h"
using namespace OFEC;

BOOST_AUTO_TEST_SUITE(CEC2013_test)


BOOST_AUTO_TEST_CASE(test_case1) {
	global::ms_global = unique_ptr<global>(new global(0.5, 0.5));
	vector<real> data(1000,0);
	vector<real> data2(1000,1);

	solution<variable<real>> sol(1, data);
	solution<variable<real>> sol2(1, data2);

	CEC2013::ShiftedElliptic_F1 a("ShiftedElliptic_F1", data.size(), 1);
	
	objective<real> temp_obj(1);
	solution<variable<real>, real> opt(a.get_optima().variable(0), std::move(temp_obj));
	a.evaluate(opt, caller::Problem);
	a.evaluate(sol, caller::Problem);
	a.evaluate(sol2, caller::Problem);
	for (auto &i : opt.get_objective())
		std::cout << i << std::endl;
	for (auto &i : sol.get_objective())
		std::cout << i << std::endl;
	for (auto &i : sol2.get_objective())
		std::cout << i << std::endl;

}



BOOST_AUTO_TEST_SUITE_END()*/