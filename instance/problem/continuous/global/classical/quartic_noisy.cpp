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
*************************************************************************/

#include "quartic_noisy.h"
namespace OFEC {
	
	quartic_noisy::quartic_noisy(param_map &v) : problem((v[param_proName]), (v[param_numDim]), 1), \
		function((v[param_proName]), (v[param_numDim]), 1) {

		set_range(-1.28, 1.28);

		initialize();
	}
	quartic_noisy::quartic_noisy(const std::string &name, size_t size_var, size_t size_obj) : problem(name, size_var, size_obj), \
		function(name, size_var, size_obj) {

		set_range(-1.28, 1.28);

		initialize();
	}

	
	void quartic_noisy::initialize() {
		set_original_global_opt();

		set_global_opt();
		m_variable_accuracy = 1.0e-2;

	}

	void quartic_noisy::evaluate__(real *x, std::vector<real>& obj) {
		if (m_translation_flag)
			translate(x);
		if (m_scale_flag)
			scale(x);
		if (m_rotation_flag)
			rotate(x);
		if (m_translation_flag)
			translate_origin(x);
		double fitness = 0;

		for (int i = 0; i < m_variable_size; i++) {
			fitness += (i + 1)*pow(x[i], 4);
		}
		fitness += global::ms_global->m_uniform[caller::Problem]->next();

		obj[0] = fitness + m_bias;

	}
	
}