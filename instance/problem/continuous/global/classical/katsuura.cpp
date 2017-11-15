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

#include "katsuura.h"
namespace OFEC {

	katsuura::katsuura(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
		function((v[param_proName]), (v[param_numDim]), 1) {

		set_range(-100., 100.);
		set_init_range(-100., 100.);
		initialize();
	}
	katsuura::katsuura(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
		function(name, size_var, 1) {
		set_range(-100., 100.);
		set_init_range(-100., 100.);
		initialize();
	}

	void katsuura::initialize() {

		set_original_global_opt();

		set_global_opt();
	}

	void katsuura::evaluate__(real *x, std::vector<real>& obj) {
		if (m_translation_flag)
			translate(x);
		if (m_scale_flag)
			scale(x);
		if (m_rotation_flag)
			rotate(x);
		if (m_translation_flag)
			translate_origin(x);

		size_t i,j;

		double temp, tmp1, tmp2, tmp3;
		obj[0] = 1.0;
		tmp3 = pow(1.0*m_variable_size, 1.2);

		for (i = 0; i<m_variable_size; i++)
		{
			temp = 0.0;
			for (j = 1; j <= 32; ++j)
			{
				tmp1 = pow(2.0, j);
				tmp2 = tmp1*x[i];
				temp += fabs(tmp2 - floor(tmp2 + 0.5)) / tmp1;
			}
			obj[0] *= pow(1.0 + (i + 1)*temp, 10.0 / tmp3);
		}
		tmp1 = 10.0 / m_variable_size / m_variable_size;
		obj[0] = obj[0] * tmp1 - tmp1;


		obj[0] += m_bias;

	}

}