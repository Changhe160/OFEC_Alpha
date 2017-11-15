/*#define BOOST_TEST_MODULE function_BBOB
#include <boost/test/unit_test.hpp>

#include "../core/problem/continuous/function.h"
#include "../instance/problem/continuous/global/BBOB/BBOB.h"
#include "../instance/problem/continuous/global/classical/FAckley.h"
#include "../instance/problem/continuous/global/classical/FRosenbrock.h"
#include "../instance/problem/continuous/global/classical/FShubert.h"
using namespace OFEC;

BOOST_AUTO_TEST_SUITE(function_BBOB_test)


BOOST_AUTO_TEST_CASE(test_case1) {
	global::ms_global = unique_ptr<global>(new global(0.5,0.5));
	vector<real> data = { 0,0,0 };
	//variable<real> x(data);
	//objective<real> obj(1);
	solution<variable<real>> sol(1,data);
	solution<variable<real>> sol2(1, data);

	for(auto &i: sol.get_variable())
	    std::cout << i << std::endl;
	
	BBOB a("FUN_BBOB_F01_Sphere", data.size(), 1);
	BBOB c("FUN_BBOB_F08_OriginalRosenbrock", data.size(), 1);
	a.get_optima();
	objective<real> temp_obj(1);
	solution<variable<real>, real> temp(a.get_optima().variable(0), std::move(temp_obj));
	a.evaluate(temp, caller::Problem);
	a.evaluate(sol, caller::Problem);
	c.evaluate(sol2, caller::Problem);
	for (auto &i : temp.get_objective())
		std::cout << i << std::endl;
	for(auto &i: sol.get_objective())
	    std::cout << i << std::endl;
	for (auto &i : sol2.get_objective())
		std::cout << i << std::endl;
	//global::ms_global.reset();
}

BOOST_AUTO_TEST_CASE(test_case2) {
	//global::ms_global = unique_ptr<global>(new global());
	vector<real> data = { 1,2,3 };
	//variable<real> x(data);
	//objective<real> obj(1);
	solution<variable<real>> sol(1, data);

	for (auto &i : sol.get_variable())
		std::cout << i << std::endl;

	BBOB b("FUN_BBOB_F22_GallagherGaussian21hiPeaks", data.size(), 1);
	b.evaluate(sol, caller::Problem);
	for (auto &i : sol.get_objective())
		std::cout << i << std::endl;
	
	//matrix b;
	//b = a.rotation();

	//for (int i = 0; i < b.data().size(); i++) {
	//	for (int j = 0; j < b.data()[i].size();j++) {
	//		std::cout << b[i][j] << ' ';
	//	}
	//	std::cout << endl;
	//}
	//global::ms_global.reset();
}

BOOST_AUTO_TEST_CASE(test_case3) {
	std::cout << "This is test_case 3 :" << std::endl;
	//global::ms_global = unique_ptr<global>(new global());
	vector<real> data = { 0,0,0 };    // Original optimal solution
	//variable<real> x(data);
	//objective<real> obj(1);
	solution<variable<real>> sol(1, data);
	FAckley a("FUN_CEC05_F08_RS_Ackley_Bound", 3, 1);
	objective<real> temp_obj(1);
	solution<variable<real>, real> temp(a.get_optima().variable(0), std::move(temp_obj));
	a.evaluate(temp,caller::Problem);
	a.evaluate(sol, caller::Problem);
	for (auto &i : temp.get_objective())
		std::cout << i << std::endl;
	for (auto &i : sol.get_objective())
		std::cout << i << std::endl;


	//global::ms_global.reset();
}


BOOST_AUTO_TEST_CASE(test_case4) {
	std::cout << "This is test_case 4 :" << std::endl;
	vector<real> data = { 1,1,1 };   // Original optimal solution

	solution<variable<real>> sol(1, data);
	FRosenbrock a("FUN_S_Rosenbrock", 3, 1);
	objective<real> temp_obj(1);
	solution<variable<real>, real> temp(a.get_optima().variable(0), std::move(temp_obj));
	a.evaluate(temp, caller::Problem);
	a.evaluate(sol, caller::Problem);
	for (auto &i : temp.get_objective())
		std::cout << i << std::endl;
	for (auto &i : sol.get_objective())
		std::cout << i << std::endl;


}

BOOST_AUTO_TEST_CASE(test_case5) {
	std::cout << "This is test_case 5 :" << std::endl;
	vector<real> data = { 1,1,1 };  

	solution<variable<real>> sol(1, data);
	FShubert a("", 3, 1);
	//objective<real> temp_obj(1);
	//solution<variable<real>, real> temp(a.get_optima().variable(0), std::move(temp_obj));
	//a.evaluate(temp, caller::Problem);
	a.evaluate(sol, caller::Problem);
	
	std::cout << a.get_optima().single_objective(0) << std::endl;
	std::cout << std::endl;
	for (auto &i : sol.get_objective())
		std::cout << i << std::endl;


}
BOOST_AUTO_TEST_SUITE_END()*/