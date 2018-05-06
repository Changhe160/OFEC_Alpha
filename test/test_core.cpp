#include "../utility/catch.hpp"

#include "../core/algorithm/individual.h"
#include "../core/algorithm/population.h"
#include "../instance/problem/continuous/multi_objective/DTLZ/DTLZ1.h"

using namespace OFEC;
void global_setup() {
	global::ms_arg.at("workingDir") = std::string("../");
}

TEST_CASE("domination relationship","[domination]")
{
	global_setup();
	global::ms_global = std::unique_ptr<global>(new global(1,0.5, 0.5));
	global::ms_global->m_problem.reset(new DTLZ1("DTLZ1", 5, 3));
	
	std::vector<double> x(global::ms_global->m_problem->variable_size(), 1), o(global::ms_global->m_problem->objective_size(), 0);
	solution<> s1(x, o);
	solution<> s2(s1);

	REQUIRE(s1.equal(s2) == true);

}



