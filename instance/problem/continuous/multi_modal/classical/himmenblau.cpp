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
*  Paper: A sequential niching memetic algorithm for continuous multimodal
*		  Appled Mathematics and Computation 218(2012) 8242-8259
*******************************************************************************************/

#include "himmenblau.h"

namespace OFEC {
	
	himmenblau::himmenblau(param_map &v) : problem((v[param_proName]), (v[param_numDim]), 1), \
		function((v[param_proName]), (v[param_numDim]), 1) {
		v[param_numDim] = 2;
		set_range(-6, 6);
		set_init_range(-6, 6);
		initialize();
	}
	himmenblau::himmenblau(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
		function(name, size_var, size_obj) {
		set_range(-6, 6);
		set_init_range(-6, 6);
		initialize();
	}

	void himmenblau::initialize() {


		m_objective_accuracy = 0.5;
		m_variable_accuracy = 1.e-4;

		// 1 gopt+3 lopt
		std::vector<std::vector<real>> var_data = { { 3.0, 2.0 },{ 3.58149, -1.8208 },{ 2.78706, 3.1282 },{ -3.76343, -3.26605 } };

		for (auto &i : var_data) {
			set_original_global_opt(i.data());
		}
		m_optima = m_original_optima;
		add_tag(problem_tag::MMP);

		//setObjSet();


	}
	void himmenblau::evaluate__(real *x, std::vector<real>& obj) {
		double s = 0;

		double t0 = (x[0] * x[0] + x[1] - 11), t1 = (x[1] * x[1] + x[0] - 7);
		s = t0*t0 + t1*t1 + 0.1*(pow(x[0] - 3, 2) + pow(x[1] - 2, 2));
		obj[0] = s + m_bias;
	}
	
}