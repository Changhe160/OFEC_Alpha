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

#include "non_continuous_rastrigin.h"
namespace OFEC {
	
	non_continuous_rastrigin::non_continuous_rastrigin(param_map &v) : problem((v[param_proName]), (v[param_numDim]), 1), \
		function((v[param_proName]), (v[param_numDim]), 1) {
		set_range(-5.12, 5.12);
		initialize();
	}
	non_continuous_rastrigin::non_continuous_rastrigin(const std::string &name, size_t size_var, size_t size_obj) : problem(name, size_var, size_obj), \
		function(name, size_var, size_obj) {
		set_range(-5.12, 5.12);
		initialize();
	}

	void non_continuous_rastrigin::initialize() {
		set_original_global_opt();
		set_global_opt();
	}

	void non_continuous_rastrigin::evaluate__(real *x, std::vector<real>& obj) {
		if (m_translation_flag)
			translate(x);
		if (m_scale_flag)
			scale(x);
		if (m_rotation_flag)
			rotate(x);
		if (m_translation_flag)
			translate_origin(x);

		real fit = 0;

		std::vector<real> y(m_variable_size);
		for (size_t i = 0; i < m_variable_size; ++i) {
			if (fabs(x[i]) < 0.5) y[i] = x[i];
			else {
				real a, b = 2 * x[i];
				if (b <= 0 && b - (int)b < 0.5) a = (int)b;
				else if (b - (int)b < 0.5) a = (int)b;
				else a = (int)b + 1;
				y[i] = a / 2;
			}
		}
		for (size_t i = 0; i < m_variable_size; ++i) {
			fit = fit + y[i] * y[i] - 10.*cos(2 * OFEC_PI*y[i]) + 10.;
		}

		obj[0] = fit + m_bias;

	}
	
}
