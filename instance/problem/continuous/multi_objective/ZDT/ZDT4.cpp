#include "ZDT4.h"

namespace OFEC {
<<<<<<< HEAD
	ZDT4::ZDT4(param_map & v) : problem(v.at("proName"), v.at("numDim"), 2), ZDT(v.at("proName"), v.at("numDim")) { //param_numDim = 10 is suggested
		v.at("numObj") = 2;
		for (int count = 1; count < (int)(v.at("numDim"));++count)
			m_domain.set_range(-5., 5., count);
		generateAdLoadPF();
=======
	ZDT4::ZDT4(param_map & v) :  ZDT4(v.at("proName"), v.at("numDim")) { //param_numDim = 10 is suggested
		
>>>>>>> zhou_li
	}
	ZDT4::ZDT4(const std::string & name, size_t size_var) : problem(name, size_var, 2), ZDT(name, size_var) {
		for (int count = 1; count < size_var; ++count)
			m_domain.set_range(-5., 5., count);
<<<<<<< HEAD
		generateAdLoadPF();
=======
		
	}
	void ZDT4::initialize() {

>>>>>>> zhou_li
	}
	void ZDT4::evaluate__(double * x, std::vector<double>& obj) {
		double g = 0;
		for (size_t n = 1; n<m_variable_size; n++)
			g = g + (pow(x[n], 2) - 10 * cos(4 * OFEC_PI*x[n]));
		g = 1 + 10 * (m_variable_size - 1) + g;
		obj[0] = x[0];
		obj[1] = g*(1 - sqrt(x[0] / g));
	}
}
