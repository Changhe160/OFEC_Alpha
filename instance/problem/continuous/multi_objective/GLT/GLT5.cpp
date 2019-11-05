#include "GLT5.h"

namespace OFEC {
	GLT5::GLT5(param_map & v) : GLT5(v.at("proName"), v.at("numDim")) { //param_numDim = 30 is suggested

	}
	GLT5::GLT5(const std::string & name, size_t size_var) : problem(name, size_var, 3), GLT(name, size_var, 3) {

	}

	void GLT5::initialize() {
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

	void GLT5::evaluate_objective(real *x, std::vector<real> &obj) {
		real yj, g = 0;
		for (size_t j = 3; j <= m_variable_size; j++) {
			yj = x[j - 1] - sin(2 * OFEC_PI*x[0] + j * OFEC_PI / m_variable_size);
			g += yj * yj;
		}

		obj[0] = (1 + g)*(1 - cos((x[0] / 2)*OFEC_PI))*(1 - cos((x[1] / 2)*OFEC_PI));
		obj[1] = (1 + g)*(1 - cos((x[0] / 2)*OFEC_PI))*(1 - sin((x[1] / 2)*OFEC_PI));
		obj[2] = (1 + g)*(1 - sin((x[0] / 2)*OFEC_PI));
	}
}