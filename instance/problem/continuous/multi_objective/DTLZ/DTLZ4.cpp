#include "DTLZ4.h"

namespace OFEC {
	DTLZ4::DTLZ4(param_map & v) : 
		DTLZ4(v.at("proName"), (v.at("numObj") + v.at("interTest1") - 1), v.at("numObj")) { // param_interTest1 = 10 is recommended
	}

	DTLZ4::DTLZ4(const std::string & name, size_t size_var, size_t size_obj) : problem(name, size_var, size_obj), \
		DTLZ(name, size_var, size_obj) {
	}

	void DTLZ4::evaluate_objective(real *x, std::vector<real> &obj) {
		real g = 0;
		for (size_t i = m_objective_size - 1; i < m_variable_size; i += 1)
			g += pow((x[i] - 0.5), 2);
		for (size_t m = 0; m < m_objective_size; m += 1)
		{
			real product = (1 + g);
			size_t i = 0;
			for (; i + m <= m_objective_size - 2; i += 1)
				product *= cos(pow(x[i], 100)*OFEC_PI / 2);
			if (m > 0)
				product *= sin(pow(x[i], 100)*OFEC_PI / 2);
			obj[m] = product;
		}
	}
}