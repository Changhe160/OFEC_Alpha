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

#include "noncont_rastrigin.h"
namespace OFEC {
	noncont_rastrigin::noncont_rastrigin(param_map &v) : problem((v[param_proName]), (v[param_numDim]), 1), \
		function((v[param_proName]), (v[param_numDim]), 1) {
		set_range(-5.12, 5.12);
		initialize();
	}
	noncont_rastrigin::noncont_rastrigin(const std::string &name, size_t size_var, size_t size_obj) : problem(name, size_var, size_obj), \
		function(name, size_var, size_obj) {
		set_range(-5.12, 5.12);
		initialize();
	}

	noncont_rastrigin::~noncont_rastrigin() {
		//dtor
	}

	void noncont_rastrigin::initialize() {
		set_original_global_opt();
		set_global_opt();
	}

	void noncont_rastrigin::evaluate__(real *x, std::vector<real>& obj) {

		double fit = 0;
		double *y = new double[m_variable_size];

		for (int i = 0; i < m_variable_size; i++) {
			if (fabs(x[i]) < 0.5) y[i] = x[i];
			else {
				double a, b = 2 * x[i];
				if (b <= 0 && b - (int)b < 0.5) a = (int)b;
				else if (b - (int)b < 0.5) a = (int)b;
				else a = (int)b + 1;
				y[i] = a / 2;
			}
		}
		for (int i = 0; i < m_variable_size; i++) {
			fit = fit + y[i] * y[i] - 10.*cos(2 * OFEC_PI*y[i]) + 10.;
		}
		delete[] y;
		y = 0;

		obj[0] = fit + m_bias;

	}
}
