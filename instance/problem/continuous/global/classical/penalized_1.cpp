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

#include "penalized_1.h"
namespace OFEC {
	
	penalized_1::penalized_1(param_map &v) :
		penalized_1((v.at("proName")), (v.at("numDim")), 1) {

	}
	penalized_1::penalized_1(const std::string &name, size_t size_var, size_t size_obj) : problem(name, size_var, size_obj), \
		function(name, size_var, size_obj) {

		
	}

	void penalized_1::initialize() {
		m_variable_monitor = true;
		set_range(-50, 50);
		set_init_range(-50., 50.);
		std::vector<real> v(m_variable_size, -1);
		set_original_global_opt(v.data());
		m_optima = m_original_optima;
		m_initialized = true;
	}

	void penalized_1::evaluate_objective(real *x, std::vector<real> &obj) {
		if (m_translation_flag)
			translate(x);
		if (m_scale_flag)
			scale(x);
		if (m_rotation_flag)
			rotate(x);
		if (m_translation_flag)
			translate_origin(x);
		std::vector<real> y(m_variable_size);
		for (int i = 0; i < m_variable_size; i++) y[i] = (x[i] + 1) / 4. + 1;
		real s = 0;
		for (int i = 0; i < m_variable_size - 1; i++)
			s += (y[i] - 1)*(y[i] - 1)*(1 + 10 * sin(OFEC_PI*y[i + 1])*sin(OFEC_PI*y[i + 1]));
		s += (y[m_variable_size - 1] - 1)*(y[m_variable_size - 1] - 1) + 10 * sin(OFEC_PI*y[0])*sin(OFEC_PI*y[0]);
		s = s*OFEC_PI / m_variable_size;
		for (int i = 0; i < m_variable_size; i++) {
			s += u(x[i], 10, 100, 4);
		}

		obj[0] = s + m_bias;

	}
	real penalized_1::u(real x, real a, real k, real m)const {
		if (x > a) return k*pow(x - a, m);
		else if (x < -a) return k*pow(-x - a, m);
		else return 0;
	}
	
}