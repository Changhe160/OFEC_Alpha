#include "GLT6.h"

namespace OFEC {
	GLT6::GLT6(param_map & v) : GLT6(v.at("proName"), v.at("numDim")) { //param_numDim = 30 is suggested

	}
	GLT6::GLT6(const std::string & name, size_t size_var) : problem(name, size_var, 3), GLT(name, size_var, 3) {

	}

	void GLT6::evaluate_objective(real *x, std::vector<real> &obj) {
		real yj, g = 0;
		for (size_t j = 3; j <= m_variable_size; j++) {
			yj = x[j - 1] - sin(2 * OFEC_PI*x[0] + j * OFEC_PI / m_variable_size);
			g += yj * yj;
		}

		obj[0] = (1 + g)*(1 - cos((x[0] / 2)*OFEC_PI))*(1 - cos((x[1] / 2)*OFEC_PI));
		obj[1] = (1 + g)*(1 - cos((x[0] / 2)*OFEC_PI))*(1 - sin((x[1] / 2)*OFEC_PI));
		obj[2] = (1 + g)*(2 - sin((x[0] / 2)*OFEC_PI) - sign(cos(4 * x[0] * OFEC_PI)));
	}
}