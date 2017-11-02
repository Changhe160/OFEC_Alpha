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

#include "non_continuous_scaffer_F6.h"
namespace OFEC {
	
	non_continuous_scaffer_F6::non_continuous_scaffer_F6(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
		scaffer_F6((v[param_proName]), (v[param_numDim]), 1) {

		set_range(-100, 100);
		initialize();
	}
	non_continuous_scaffer_F6::non_continuous_scaffer_F6(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
		scaffer_F6(name, size_var, size_obj) {

		set_range(-100, 100);
		initialize();
	}

	void non_continuous_scaffer_F6::initialize() {

	}

	void non_continuous_scaffer_F6::evaluate__(real *x, std::vector<real>& obj) {
		if (m_translation_flag)
			translate(x);
		if (m_scale_flag)
			scale(x);
		if (m_rotation_flag)
			rotate(x);
		if (m_translation_flag)
			translate_origin(x);

		real fitness = 0;
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
			real result001 = 0;
			real x001 = y[i];
			real x002 = y[(i + 1) % m_variable_size];
			result001 = 0.5 + (pow(sin(sqrt(x001*x001 + x002*x002)), 2.0) - 0.5) / pow((1 + 0.001*(x001*x001 + x002*x002)), 2.0);
			fitness += result001;
		}
		obj[0] = fitness + m_bias;
	}
	
}