#include "../utility/catch.hpp"
#include "../run/include_problem.h"
#include <iomanip>

using namespace OFEC;

TEST_CASE("MOP", "[WFG]")
{
    global::ms_global = std::unique_ptr<global>(new global(1, 0.5, 0.5));
    size_t size_var, num_obj;

    size_var = 8, num_obj = 3;
    global::ms_global->m_problem.reset(new WFG1("MOP_WFG1", size_var, num_obj));
    global::ms_global->m_problem->initialize();
    dynamic_cast<WFG1*>(global::ms_global->m_problem.get())->set_k(4);

    solution<variable_vector<real>,real> s(num_obj, 0, size_var);
	for (int i = 0; i < size_var; ++i) {
		s.variable()[i] = 2 * (i + 1);
	}
	s.evaluate(false, caller::Problem);

	for (int i = 0; i < s.objective_size(); ++i)
		std::cout << std::setprecision(6) << s.objective(i) << " ";
	std::cout << std::endl;
}
