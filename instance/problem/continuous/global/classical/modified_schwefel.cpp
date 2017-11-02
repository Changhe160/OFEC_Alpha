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

#include "modified_schwefel.h"
namespace OFEC {

	modified_schwefel::modified_schwefel(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
		function((v[param_proName]), (v[param_numDim]), 1) {

		set_range(-100., 100.);
		initialize();
	}
	modified_schwefel::modified_schwefel(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
		function(name, size_var, 1) {
		set_range(-100., 100.);
		initialize();
	}

	void modified_schwefel::initialize() {

		set_original_global_opt();

		set_global_opt();
	}

	void modified_schwefel::evaluate__(real *x, std::vector<real>& obj) {
		if (m_translation_flag)
			translate(x);
		if (m_scale_flag)
			scale(x);
		if (m_rotation_flag)
			rotate(x);
		if (m_translation_flag)
			translate_origin(x);

		size_t i;

		real tmp;
		obj[0] = 0.0;

		for (i = 0; i<m_variable_size; i++)
		{
			x[i] += 4.209687462275036e+002;
			if (x[i]>500)
			{
				obj[0] -= (500.0 - fmod(x[i], 500))*sin(pow(500.0 - fmod(x[i], 500), 0.5));
				tmp = (x[i] + 500.0) / 100;
				obj[0] += tmp*tmp / m_variable_size;
			}
			else if (x[i]<-500)
			{
				obj[0] -= (-500.0 + fmod(fabs(x[i]), 500))*sin(pow(500.0 - fmod(fabs(x[i]), 500), 0.5));
				tmp = (x[i] + 500.0) / 100;
				obj[0] += tmp*tmp / m_variable_size;
			}
			else
				obj[0] -= x[i] * sin(pow(fabs(x[i]), 0.5));
		}
		obj[0] += 4.189828872724338e+002*m_variable_size;
		obj[0] += m_bias;

	}

}