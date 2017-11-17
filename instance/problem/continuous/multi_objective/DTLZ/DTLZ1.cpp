#include "DTLZ1.h"

namespace OFEC {
	DTLZ1::DTLZ1(param_map & v) : problem(v[param_proName], (v[param_numObj] + v[param_interTest1] - 1), v[param_numObj]), \
		DTLZ(v[param_proName], (v[param_numObj] + v[param_interTest1] - 1), v[param_numObj]) {
	}

	DTLZ1::DTLZ1(const std::string & name, size_t size_var, size_t size_obj) : problem(name, size_var, size_obj), \
		DTLZ(name, size_var, size_obj) {
	}

	void DTLZ1::evaluate__(double * x, std::vector<double>& obj) {
		double g = 0;
		for (size_t i = m_objective_size - 1; i < m_variable_size; i += 1)
		{
			g += (x[i] - 0.5)*(x[i] - 0.5) - cos(20 * OFEC_PI*(x[i] - 0.5));
		}
		g = (m_variable_size + 1 - m_objective_size + g) * 100;
		for (size_t m = 0; m < m_objective_size; m += 1)
		{
			double product = 0.5*(1 + g);
			size_t i = 0;
			for (; m_objective_size >= 2 + m && i <= m_objective_size - 2 - m; i += 1)
				product *= x[i];
			if (m > 0)
				product *= (1 - x[i]);
			obj[m] = product;
		}
	}
}