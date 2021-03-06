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
// Created: 21 July 2011
// Last modified:
#include "elliptic.h"
namespace OFEC {
	
	elliptic::elliptic(param_map &v) :
		elliptic((v.at("proName")), (v.at("numDim")), 1) {
		
	}
	elliptic::elliptic(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
		function(name, size_var, size_obj) {

		
	}

	void elliptic::initialize() {
		m_variable_monitor = true;
		set_range(-100., 100.);
		set_init_range(-100., 100.);

		set_original_global_opt();
		set_global_opt();
		//setObjSet();
		m_initialized = true;
	}



	void elliptic::evaluate_objective(real *x, std::vector<real> &obj) {
		if (m_translation_flag)
			translate(x);
		if (m_scale_flag)
			scale(x);
		if (m_rotation_flag)
			rotate(x);
		if (m_translation_flag)
			translate_origin(x);
		real s = 0;
		for (int i = 0; i < m_variable_size; i++) {
			s += pow(1e6, i / (m_variable_size - 1.))*x[i] * x[i];
		}
		obj[0] = s + m_bias;
	}
	
}