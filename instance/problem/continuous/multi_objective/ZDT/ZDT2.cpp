#include "ZDT2.h"

namespace OFEC {
	ZDT2::ZDT2(param_map & v) : problem(v[param_proName], v[param_numDim], 2), ZDT(v) {
		generateAdLoadPF();
	}
	void ZDT2::evaluate__(double * x, std::vector<double>& obj) {
		double g = 0;
		for (size_t n = 1; n<m_variable_size; n++)
			g = g + x[n];
		g = 1 + 9 * g / (m_variable_size - 1);
		obj[0] = x[0];
		obj[1] = g*(1 - pow(obj[0] / g, 2));
	}
}