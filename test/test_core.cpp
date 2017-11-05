#define BOOST_TEST_MODULE core
#include <boost/test/unit_test.hpp>

#include "../core/algorithm/individual.h"
#include "../core/algorithm/population.h"

using namespace OFEC;

BOOST_AUTO_TEST_SUITE(core_test)


BOOST_AUTO_TEST_CASE(test_case1)
{
	try {
		THROW("helel");
	}
	catch (myexcept e) {
		std::cout<<e.what();
	}

	std::vector<double> a = { 1,1 }, b = { 0,0 };
	solution<> s1(b.size(), a.size());
	solution<> s2(s1);

	if (s1.dominate( s2)) {}
	if (s1.dominate_equal( s2)) {}

	double d = s1.objective_distance(s2);
}

BOOST_AUTO_TEST_CASE(test_case2)
{
	individual<> i;
	i.initialize(0);
	population<individual<>> p;


}

BOOST_AUTO_TEST_CASE(test_case3)
{


}

BOOST_AUTO_TEST_SUITE_END()

