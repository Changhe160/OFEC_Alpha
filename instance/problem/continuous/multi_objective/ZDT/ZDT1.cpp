#include "ZDT1.h"

namespace OFEC {
	ZDT1::ZDT1(param_map & v) : problem(v[param_proName], v[param_numDim], 2), ZDT(v) {
		generateAdLoadPF();
	}
	void ZDT1::evaluate__(double * x, std::vector<double>& obj) {
		double g = 0;
		for (size_t n = 1; n<m_variable_size; n++)
			g = g + x[n];
		g = pow(g / (m_variable_size - 1), 0.25);
		g = 1 + 9 * g;
		obj[0] = 1 - exp(-4 * x[0])*pow(sin(6 * OFEC_PI*x[0]), 6);
		obj[1] = g*(1 - pow(obj[0] / g, 2));
	}
}