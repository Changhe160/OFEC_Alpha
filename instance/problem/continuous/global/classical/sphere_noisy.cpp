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
	
	sphere_noisy::sphere_noisy(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
		sphere((v[param_proName]), (v[param_numDim]), 1) {

		initialize();

	}
	sphere_noisy::sphere_noisy(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
		sphere(name, size_var, size_obj) {


		initialize();
	}

	void sphere_noisy::initialize() {

	}


	void sphere_noisy::evaluate__(real *x, std::vector<real>& obj) {
		if (m_translation_flag)
			translate(x);
		if (m_scale_flag)
			scale(x);
		if (m_rotation_flag)
			rotate(x);
		if (m_translation_flag)
			translate_origin(x);

		real fit = 0;

		double noise;
		for (int i = 0; i < m_variable_size; i++) {
			noise = 0.01*global::ms_global->m_uniform[caller::Problem]->next();
			fit += (x[i] + noise)*(x[i] + noise);
		}
		obj[0] = fit + m_bias;

	}
	
}