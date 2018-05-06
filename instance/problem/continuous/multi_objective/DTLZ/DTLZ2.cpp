#include "DTLZ2.h"

namespace OFEC {
	DTLZ2::DTLZ2(param_map & v) : 
		DTLZ2(v.at("proName"), (v.at("numObj") + v.at("interTest1") - 1), v.at("numObj")) { // param_interTest1 = 5 is recommended
	}

	DTLZ2::DTLZ2(const std::string & name, size_t size_var, size_t size_obj) : problem(name, size_var, size_obj), \
		DTLZ(name, size_var, size_obj) {
	}

	void DTLZ2::evaluate__(double * x, std::vector<double>& obj) {
		double g = 0;
		for (size_t i = m_objective_size - 1; i < m_variable_size; i += 1)
			g += pow((x[i] - 0.5), 2);
		for (size_t m = 0; m < m_objective_size; m += 1)
		{
			double product = (1 + g);
			size_t i = 0;
			for (; i + m <= m_objective_size - 2; i += 1)
				product *= cos(x[i] * OFEC_PI / 2);
			if (m > 0)
				product *= sin(x[i] * OFEC_PI / 2);
			obj[m] = product;
		}
	}
}