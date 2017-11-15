
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

#include "bent_cigar.h"
namespace OFEC {

	bent_cigar::bent_cigar(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
		function((v[param_proName]), (v[param_numDim]), 1) {
		set_range(-100., 100.);
		set_init_range(-100., 100.);
		initialize();
	}
	bent_cigar::bent_cigar(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
		function(name, size_var, 1) {
		set_range(-100., 100.);
		set_init_range(-100., 100.);
		initialize();
	}

	void bent_cigar::initialize() {

		set_original_global_opt();

		set_global_opt();
	}

	void bent_cigar::evaluate__(real *x, std::vector<real>& obj) {
		if (m_translation_flag)
			translate(x);
		if (m_scale_flag)
			scale(x);
		if (m_rotation_flag)
			rotate(x);
		if (m_translation_flag)
			translate_origin(x);

		size_t i;

		obj[0] = x[0] * x[0];
		for (i = 1; i< m_variable_size; ++i)
		{
			obj[0] += pow(10.0, 6.0) * x[i] * x[i];
		}

		obj[0] += m_bias;

	}

}