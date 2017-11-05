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

#include "penalized_2.h"
namespace OFEC {
	
	penalized_2::penalized_2(param_map &v) : problem((v[param_proName]), (v[param_numDim]), 1), \
		function((v[param_proName]), (v[param_numDim]), 1) {

		set_range(-50, 50);

		initialize();
	}
	penalized_2::penalized_2(const std::string &name, size_t size_var, size_t size_obj) : problem(name, size_var, size_obj), \
		function(name, size_var, size_obj) {

		set_range(-50, 50);

		initialize();
	}

	void penalized_2::initialize() {
		std::vector<real> v(m_variable_size, 1.0);
		set_original_global_opt(v.data());
		m_optima = m_original_optima;
	}


	void penalized_2::evaluate__(real *x, std::vector<real>& obj) {
		if (m_translation_flag)
			translate(x);
		if (m_scale_flag)
			scale(x);
		if (m_rotation_flag)
			rotate(x);
		if (m_translation_flag)
			translate_origin(x);
		double s = 0;

		for (int i = 0; i < m_variable_size - 1; i++)
			s += (x[i] - 1)*(x[i] - 1)*(1 + sin(3 * OFEC_PI*x[i + 1])*sin(3 * OFEC_PI*x[i + 1]));
		s += (x[m_variable_size - 1] - 1)*(x[m_variable_size - 1] - 1)*(1 + sin(2 * OFEC_PI*x[m_variable_size - 1])*sin(2 * OFEC_PI*x[m_variable_size - 1])) + sin(3 * OFEC_PI*x[0])*sin(3 * OFEC_PI*x[0]);
		s = s*0.1;
		for (int i = 0; i < m_variable_size; i++)
			s += u(x[i], 5, 100, 4);

		obj[0] = s + m_bias;

	}
	double penalized_2::u(real x, double a, double k, double m)const {
		if (x > a) return k*pow(x - a, m);
		else if (x < -a) return k*pow(-x - a, m);
		else return 0;
	}
	
}
