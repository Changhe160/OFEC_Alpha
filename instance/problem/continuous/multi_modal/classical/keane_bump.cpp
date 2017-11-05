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
******************************************************************************************
*  Paper; Multimodal Optimization by Means of a Topological Species Conservation Algorithm
*		  IEEE TRANSACTIONS ON EVOLUTIONARY COMPUTATION, VOL.14,NO.6,DECEMBER 2010
*******************************************************************************************/

#include "keane_bump.h"

namespace OFEC {
	
	keane_bump::keane_bump(param_map &v) : problem((v[param_proName]), (v[param_numDim]), 1), \
		function((v[param_proName]), (v[param_numDim]), 1) {

		set_range(0, 10);
		initialize();
	}
	keane_bump::keane_bump(const std::string &name, size_t size_var, size_t size_obj) : problem(name, size_var, size_obj), \
		function(name, size_var, size_obj) {

		set_range(0, 10);
		initialize();
	}

	void keane_bump::initialize() {
		m_opt_mode[0] = optimization_mode::Maximization;
		m_objective_accuracy = 0.5;
		m_variable_accuracy = 1.e-4;

		m_original_optima.set_number_variable(1);
		m_optima = m_original_optima;
	}

	void keane_bump::evaluate__(real *x, std::vector<real>& obj) {

		double s, a = 0, b = 1, c = 0;
		int i;
		for (i = 0; i < m_variable_size; i++) {
			a += pow(cos(x[i]), 4);
			b *= cos(x[i])*cos(x[i]);
			c += i*x[i] * x[i];
		}
		s = abs(a - 2 * b) / sqrt(c);
		obj[0] = s;

	}
	
}