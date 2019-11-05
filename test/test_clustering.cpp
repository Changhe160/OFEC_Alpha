#include "../utility/catch.hpp"
#include "../core/algorithm/population.h"
#include "../core/algorithm/individual.h"
#include "../instance/problem/continuous/global/classical/sphere.h"
#include "../utility/clustering/hslh.h"

using namespace OFEC;

TEST_CASE("hierarchical culstering", "[cluster]") //clustering
{
	global::ms_global.reset(new global(1, 0.5, 0.5));
	global::ms_global->m_problem.reset(new sphere("GOP_CLASSICAL_sphere", 10,1));
	global::ms_global->m_problem->initialize();

	population<individual<>> p(100,10);
	p.initialize();
	for (int i = 0; i < p.size(); ++i) {
		p[i].evaluate(false);
	}

	HSLH<individual<>> c(p);

	c.clustering(5);
}


