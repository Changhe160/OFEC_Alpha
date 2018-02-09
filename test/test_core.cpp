#include "../utility/catch.hpp"

#include "../core/algorithm/individual.h"
#include "../core/algorithm/population.h"
#include "../instance/problem/continuous/multi_objective/DTLZ/DTLZ1.h"

using namespace OFEC;

TEST_CASE("domination relationship","[domination]")
{
	global::ms_global = std::unique_ptr<global>(new global(1,0.5, 0.5));
	DTLZ1 f1("DTLZ1", 2, 2);

	std::vector<double> a = { 1,1 }, b = { 0,0 };
	solution<> s1(b, a);
	solution<> s2(s1);

	REQUIRE(s1.dominate(s2) == true);
}



