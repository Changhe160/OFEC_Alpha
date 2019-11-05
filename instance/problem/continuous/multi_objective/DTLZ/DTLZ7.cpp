#include "DTLZ7.h"
 
namespace OFEC {
	DTLZ7::DTLZ7(param_map & v) : DTLZ7(v.at("proName"), (v.at("numObj") + v.at("interTest1") - 1), v.at("numObj")) { // param_interTest1 = 20 is recommended
	}

	DTLZ7::DTLZ7(const std::string & name, size_t size_var, size_t size_obj) : problem(name, size_var, size_obj), DTLZ(name, size_var, size_obj) {}

	void DTLZ7::evaluate_objective(real *x, std::vector<real> &obj) {
		int i = 0;
		int j = 0;
		int n = m_variable_size;
		int k = n - m_objective_size + 1;
		real g = 0;
		real h = 0;
		for (i = n - k + 1; i <= n; i++)
			g += x[i - 1];
		g = 1 + 9 * g / k;
		for (i = 1; i <= m_objective_size - 1; i++)
			obj[i - 1] = x[i - 1];
		for (j = 1; j <= m_objective_size - 1; j++)
			h += x[j - 1] / (1 + g) * (1 + sin(3 * OFEC_PI * x[j - 1]));
		h = m_objective_size - h;
		obj[m_objective_size - 1] = (1 + g) * h;
	}
}