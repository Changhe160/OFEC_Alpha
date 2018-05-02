#include "DTLZ3.h"

namespace OFEC {
	DTLZ3::DTLZ3(param_map & v) : problem(v.at("proName"), (v.at("numObj") + v.at("interTest1") - 1), v.at("numObj")), \
		DTLZ(v.at("proName"), (v.at("numObj") + v.at("interTest1") - 1), v.at("numObj")) {
	}

	DTLZ3::DTLZ3(const std::string & name, size_t size_var, size_t size_obj) : problem(name, size_var, size_obj), \
		DTLZ(name, size_var, size_obj) {
	}

	void DTLZ3::evaluate__(double * x, std::vector<double>& obj) {
		double g = 0;
		for (size_t i = m_objective_size - 1; i < m_variable_size; i += 1)
			g += (pow((x[i] - 0.5), 2) - cos(20 * OFEC_PI*(x[i] - 0.5)));
		g = (10 + g) * 100;
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