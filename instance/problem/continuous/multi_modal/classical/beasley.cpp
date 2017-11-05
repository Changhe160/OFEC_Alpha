/*************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Changhe Li
* Email: changhe.lw@gmail.com 
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
******************************************************************************************
*  Paper; A sequential niching memetic algorithm for continuous multimodal
*		  Appled Mathematics and Computation 218(2012) 8242-8259
*******************************************************************************************/
/*******************************************************************************************
*  Paper: A sequential niching memetic algorithm for continuous multimodal
*		  Appled Mathematics and Computation 218(2012) 8242-8259
****************************************************************************************
*  LaTex:F_4(x)=\mathrm{e}^{-2\lg2*{{\frac{x-0.08}{0.854}}^2}}*{\sin{5\pi*{x^0.75-0.05}}}^6
*******************************************************************************************/

#include "beasley.h"

namespace OFEC {
	
	beasley::beasley(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
		function((v[param_proName]), (v[param_numDim]), 1) {

		v[param_numDim] = 1;
		set_range(0, 1.); // note
		initialize();
	}
	beasley::beasley(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
		function(name, size_var, size_obj) {
		set_range(0, 1.); // note
		initialize();
	}

	void beasley::initialize() { // note
		m_opt_mode[0] = optimization_mode::Maximization;

		m_original_optima.set_number_variable(5); //1 gopt+ 4 lopt
		m_variable_accuracy = 0.1;
		m_objective_accuracy = 1.e-6;
		std::vector<std::vector<real>> var_data = { {0.08}, {0.25}, {0.45}, {0.68}, {0.93} };
		for (auto &i : var_data) {
			set_original_global_opt(i.data());
		}
		m_optima = m_original_optima;
		add_tag(problem_tag::MMP);
		//setObjSet();
	}
	void beasley::evaluate__(real *x, std::vector<real>& obj) {

		double s;
		s = exp(-2 * log(2.)*((x[0] - 0.08) / 0.854)*((x[0] - 0.08) / 0.854))*pow(sin(5 * OFEC_PI*(pow(x[0], 0.75) - 0.05)), 6);
		obj[0] = s + m_bias;  // note

	}
	
}