#include "GLT4.h"

namespace OFEC {
	GLT4::GLT4(param_map & v) : GLT4(v.at("proName"), v.at("numDim")) { //param_numDim = 30 is suggested

	}
	GLT4::GLT4(const std::string & name, size_t size_var) : problem(name, size_var, 2), GLT(name, size_var, 2) {

	}

	void GLT4::initialize() {
		std::vector<std::pair<real, real>> r;
		r.push_back(std::make_pair(0., 1.));
		r.push_back(std::make_pair(0., 1.));
		for (int i = 2; i < m_variable_size; ++i) {
			r.push_back(std::make_pair(-1., 1.));
		}
		set_init_range(r);
		set_range(r);
		load_PF();
	}

	void GLT4::evaluate_objective(real *x, std::vector<real> &obj) {
		real yj, g = 0;
		for (size_t j = 2; j <= m_variable_size; j++) {
			yj = x[j - 1] - sin(2 * OFEC_PI*x[0] + j * OFEC_PI / m_variable_size);
			g += yj * yj;
		}

		obj[0] = (1 + g)*x[0];
		obj[1] = (1 + g)*(2 - 2 * std::sqrt(x[0]) * cos(2 * std::sqrt(x[0])*OFEC_PI)*cos(2 * sqrt(x[0])*OFEC_PI));
	}
}