
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

#include "rotated_discus.h"
namespace OFEC {

	rotated_discus::rotated_discus(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
		discus((v[param_proName]), (v[param_numDim]), 1) {
		initialize();
	}
	rotated_discus::rotated_discus(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
		discus(name, size_var, 1) {
		initialize();
	}

	void rotated_discus::initialize() {

		load_rotation("instance/problem/continuous/global/CEC2005/data/");
		m_optima.clear();
		set_global_opt();
	}

	void rotated_discus::evaluate__(real *x, std::vector<real>& obj) {
		discus::evaluate__(x, obj);

	}

}