#include "ZDT3.h"

namespace OFEC {
	ZDT3::ZDT3(param_map & v) : problem(v.at("proName"), v.at("numDim"), 2), ZDT(v.at("proName"), v.at("numDim")) {
		
	}
	ZDT3::ZDT3(const std::string & name, size_t size_var) : problem(name, size_var, 2), ZDT(name, size_var) {
		
	}
	void ZDT3::initialize() {
		ZDT::initialize();
		generateAdLoadPF();
	}
	void ZDT3::evaluate__(double * x, std::vector<double>& obj) {
		double g = 0;
		for (size_t n = 1; n<m_variable_size; n++)
			g = g + x[n];
		g = 1 + 9 * g / (m_variable_size - 1);
		obj[0] = x[0];
		obj[1] = g*(1 - sqrt(x[0] / g) - x[0] * sin(10 * x[0] * OFEC_PI) / g);
	}
}
