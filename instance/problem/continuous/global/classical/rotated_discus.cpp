
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

	rotated_discus::rotated_discus(param_map &v) :
		rotated_discus((v.at("proName")), (v.at("numDim")), 1) {
		
	}
	rotated_discus::rotated_discus(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
		discus(name, size_var, 1) {
		
	}

	void rotated_discus::initialize() {
		m_variable_monitor = true;
		set_range(-100., 100.);
		set_init_range(-100., 100.);

		set_original_global_opt();

		load_rotation("instance/problem/continuous/global/CEC2005/data/");

		set_global_opt();
	}

	void rotated_discus::evaluate__(real *x, std::vector<real>& obj) {
		discus::evaluate__(x, obj);

	}

}