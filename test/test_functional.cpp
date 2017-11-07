#define BOOST_TEST_MODULE functional
#include <boost/test/unit_test.hpp>

#include "../utility/functional.h"
#include "../core/algorithm/solution.h"

using namespace OFEC;

BOOST_AUTO_TEST_SUITE(functional_test)

BOOST_AUTO_TEST_CASE(test_case1) 
{
	std::vector<double> a = { 1,1 }, b = { 0,0 };

	double dis = euclidean_distance(a.begin(),a.end(),b.begin());
	//BOOST_CHECK_EQUAL(dis, sqrt(2));
	BOOST_CHECK_CLOSE(dis, sqrt(2), 1.0e-6);

	objective<> obj(a),ob2(b);

	//obj.distance2Obj(b);
	//obj.distance2Obj(b, euclidean_distance<vector<double>::const_iterator, vector<double>::const_iterator>);
//	obj.distance2Obj(b);
	//obj.compare(b,optimization_mode::Minimization);
	//obj.compare(ob2, optimization_mode::Maximization);
}



BOOST_AUTO_TEST_SUITE_END()

