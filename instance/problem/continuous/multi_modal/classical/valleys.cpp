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
*  Paper: Multimodal Optimization by Means of a Topological Species Conservation Algorithm
*		  IEEE TRANSACTIONS ON EVOLUTIONARY COMPUTATION, VOL.14,NO.6,DECEMBER 2010
*******************************************************************************************/

#include "valleys.h"

namespace OFEC {
	
	valleys::valleys(param_map &v) : problem((v[param_proName]), (v[param_numDim]), 1), \
		function((v[param_proName]), (v[param_numDim]), 1) {
		v[param_numDim] = 2;
		std::vector<std::pair<real, real>> data;
		data.push_back(std::make_pair(-2.5, 3));
		data.push_back(std::make_pair(-2, 2));

		set_range(data);
		set_range(std::move(data)); 
		initialize();
	}
	valleys::valleys(const std::string &name, size_t size_var, size_t size_obj) : problem(name, size_var, size_obj), \
		function(name, size_var, size_obj) {
		std::vector<std::pair<real, real>> data;
		data.push_back(std::make_pair(-2.5, 3));
		data.push_back(std::make_pair(-2, 2));

		set_range(data);
		set_range(std::move(data));
		initialize();
	}

	void valleys::initialize() {
		m_opt_mode[0] = optimization_mode::Maximization;

		m_objective_accuracy = 0.5;
		m_variable_accuracy = 1.e-4;

		m_original_optima.set_number_variable(2); //1 gopt + 1 lopt
		std::vector<std::vector<real>> var_data = { { 1.69714, 0.0 }, {-1.44446, 0.0 } };

		for (auto &i : var_data) {
			set_original_global_opt(i.data());
		}
		m_optima = m_original_optima;
		add_tag(problem_tag::MMP);
		//setObjSet();
	}
	void valleys::evaluate__(real *x, std::vector<real>& obj) {
		double s;

		s = sin(2 * x[0] - 0.5*OFEC_PI) + 3 * cos(x[1]) + 0.5*x[0];
		obj[0] = s;

	}
	
}