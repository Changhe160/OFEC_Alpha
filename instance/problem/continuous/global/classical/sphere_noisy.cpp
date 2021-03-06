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

#include "sphere_noisy.h"
namespace OFEC {
	
	sphere_noisy::sphere_noisy(param_map &v) :
		sphere_noisy((v.at("proName")), (v.at("numDim")), 1) {

	}
	sphere_noisy::sphere_noisy(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
		function(name, size_var, size_obj) {

	}

	void sphere_noisy::initialize() {
		m_variable_monitor = true;
		set_range(-100., 100.);
		set_init_range(-100., 100.);
		set_original_global_opt();

		set_global_opt();
		m_initialized = true;
	}


	void sphere_noisy::evaluate_objective(real *x, std::vector<real> &obj) {
		if (m_translation_flag)
			translate(x);
		if (m_scale_flag)
			scale(x);
		if (m_rotation_flag)
			rotate(x);
		if (m_translation_flag)
			translate_origin(x);

		real fit = 0;

		real noise;
		for (int i = 0; i < m_variable_size; i++) {
			noise = 0.01*global::ms_global->m_uniform[caller::Problem]->next();
			fit += (x[i] + noise)*(x[i] + noise);
		}
		obj[0] = fit + m_bias;

	}
	
}