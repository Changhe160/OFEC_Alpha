/*************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Changhe Li and Li Zhou
* Email: changhe.lw@gmail.com, 441837060@qq.com
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*************************************************************************/

#include "HGBat.h"
namespace OFEC {

	HGBat::HGBat(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
		function((v[param_proName]), (v[param_numDim]), 1) {

		set_range(-100., 100.);
		set_init_range(-100., 100.);
		initialize();
	}
	HGBat::HGBat(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
		function(name, size_var, 1) {
		set_range(-100., 100.);
		set_init_range(-100., 100.);
		initialize();
	}

	void HGBat::initialize() {

		set_original_global_opt();

		set_global_opt();
	}

	void HGBat::evaluate__(real *x, std::vector<real>& obj) {
		if (m_translation_flag)
			translate(x);
		if (m_scale_flag)
			scale(x);
		if (m_rotation_flag)
			rotate(x);
		if (m_translation_flag)
			translate_origin(x);

		size_t i;

		double alpha, r2, sum_x;
		alpha = 1.0 / 4.0;

		r2 = 0.0;
		sum_x = 0.0;
		for (i = 0; i<m_variable_size; ++i)
		{
			x[i] = x[i] - 1.0;//shift to orgin
			r2 += x[i] * x[i];
			sum_x += x[i];
		}
		obj[0] = pow(fabs(pow(r2, 2.0) - pow(sum_x, 2.0)), 2 * alpha) + (0.5*r2 + sum_x) / m_variable_size + 0.5;

		obj[0] += m_bias;

	}

}