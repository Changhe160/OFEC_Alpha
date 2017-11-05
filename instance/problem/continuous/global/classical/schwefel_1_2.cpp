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

#include "schwefel_1_2.h"
namespace OFEC {
	
	schwefel_1_2::schwefel_1_2(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
		function((v[param_proName]), (v[param_numDim]), 1) {

		set_range(-100, 100);

		initialize();
	}
	schwefel_1_2::schwefel_1_2(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
		function(name, size_var, size_obj) {

		set_range(-100, 100);

		initialize();
	}

	void schwefel_1_2::initialize() {

		set_original_global_opt();

		set_global_opt();

	}


	void schwefel_1_2::evaluate__(real *x, std::vector<real>& obj) {
		if (m_translation_flag)
			translate(x);
		if (m_scale_flag)
			scale(x);
		if (m_rotation_flag)
			rotate(x);
		if (m_translation_flag)
			translate_origin(x);
		double s1 = 0, s2 = 0;

		for (int i = 0; i < m_variable_size; i++) {
			for (int j = 0; j <= i; j++)
				s1 += x[j];
			s2 += s1*s1;
			s1 = 0;
		}

		obj[0] = s2 + m_bias;

	}
	
}