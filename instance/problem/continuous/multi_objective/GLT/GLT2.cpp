#include "GLT2.h"

namespace OFEC {
	GLT2::GLT2(param_map & v) : GLT2(v.at("proName"), v.at("numDim")) { //param_numDim = 30 is suggested

	}
	GLT2::GLT2(const std::string & name, size_t size_var) : problem(name, size_var, 2), GLT(name, size_var, 2) {

	}

	void GLT2::evaluate_objective(real *x, std::vector<real> &obj) {
		real yj = 0 , g = 0;
		for (size_t j = 2; j <= m_variable_size; j++) {
			yj = x[j - 1] - sin(2 * OFEC_PI*x[0] + j * OFEC_PI / m_variable_size);
			g += yj * yj;
		}

		obj[0] = (1 + g)*(1 - cos(OFEC_PI*x[0] / 2));
		obj[1] = (1 + g)*(10 - 10 * sin(OFEC_PI*x[0] / 2));
	}
}