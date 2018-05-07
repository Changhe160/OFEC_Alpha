#include "ZDT1.h"

namespace OFEC {
<<<<<<< HEAD
	ZDT1::ZDT1(param_map & v) : problem(v.at("proName"), v.at("numDim"), 2), ZDT(v.at("proName"), v.at("numDim")) { //param_numDim = 30 is suggested
		v.at("numObj") = 2;
		generateAdLoadPF();
	}
	ZDT1::ZDT1(const std::string & name, size_t size_var) : problem(name, size_var, 2), ZDT(name, size_var) {
		generateAdLoadPF();
=======
	ZDT1::ZDT1(param_map & v) : ZDT1(v.at("proName"), v.at("numDim")) { //param_numDim = 30 is suggested
		
	}
	ZDT1::ZDT1(const std::string & name, size_t size_var) : problem(name, size_var, 2), ZDT(name, size_var) {
		
	}
	void ZDT1::initialize() {

>>>>>>> zhou_li
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