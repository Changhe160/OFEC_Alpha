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

#include "shaffer.h"
namespace OFEC {

	shaffer::shaffer(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
		function((v[param_proName]), (v[param_numDim]), 1) {
		v[param_numDim] = 2;
		set_range(-15, 15);
		set_init_range(-15, 15);
		initialize();
	}
	shaffer::shaffer(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
		function(name, size_var, size_obj) {
		set_range(-15, 15);
		set_init_range(-15, 15);
		initialize();
	}

	void shaffer::initialize() {
		m_variable_accuracy = 1.e-6;
		m_objective_accuracy = 0.1;
		m_opt_mode[0] = optimization_mode::Maximization;

		m_original_optima.set_number_variable(1);
		std::vector<real> var_data = { 0.0, 0.0 };
		set_original_global_opt(var_data.data());

		m_optima = m_original_optima;
		//setObjSet();
	}
	void shaffer::evaluate__(real *x, std::vector<real>& obj) {
		if (m_translation_flag)
			translate(x);
		if (m_scale_flag)
			scale(x);
		if (m_rotation_flag)
			rotate(x);
		if (m_translation_flag)
			translate_origin(x);
		double s, t = x[0] * x[0] + x[1] * x[1];
		s = 0.5 + (0.5 - pow(sin(sqrt(0.0001 + t)), 2)) / pow(1 + 0.001*t*t, 2);
		obj[0] = s + m_bias;

	}
	
}