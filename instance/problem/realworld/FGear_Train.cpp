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

#include "FGear_Train.h"
namespace OFEC {
	FGear_Train::FGear_Train(param_map &v) :problem((v[param_proName]), 4, 1), \
		function((v[param_proName]), 4, 1) {
		set_range(12, 60); 
		set_init_range(12, 60);
		initialize();

	}

	FGear_Train::FGear_Train(const std::string &name, size_t size_var, size_t size_obj) :problem(name, 4, 1), \
		function(name, 4, 1) {
		set_range(12, 60);
		set_init_range(12, 60);
		initialize();
	}

	void FGear_Train::initialize() {
		std::vector<real> v(m_variable_size, 0);
		v[0] = 15; v[1] = 20; v[2] = 57; v[3] = 59;

		set_original_global_opt(v.data());
		m_optima = m_original_optima;
	}


	void FGear_Train::evaluate__(real *x, std::vector<real>& obj) {

		int x1, x2, x3, x4;
		real s;

		x1 = (int)x[0]; x2 = (int)x[1]; x3 = (int)x[2]; x4 = (int)x[3];
		s = 1. / 6.931 - x1*x2 / (double)(x3*x4);

		obj[0] = s*s;

	}
}