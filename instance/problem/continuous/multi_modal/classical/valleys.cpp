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
	
	valleys::valleys(param_map &v) :
		valleys((v.at("proName")), 2, 1) {
	
		
	}
	valleys::valleys(const std::string &name, size_t size_var, size_t size_obj) : problem(name, size_var, size_obj), \
		function(name, size_var, size_obj) {
	
	}

	void valleys::initialize() {
		std::vector<std::pair<real, real>> data;
		data.push_back(std::make_pair(-2.5, 3));
		data.push_back(std::make_pair(-2, 2));

		set_range(data);
		set_init_range(std::move(data));
		m_opt_mode[0] = optimization_mode::Maximization;

		m_objective_accuracy = 0.5;
		m_variable_accuracy = 1.e-4;
		m_variable_monitor = true;
		 //1 gopt + 1 lopt
		std::vector<std::vector<real>> var_data = { { 1.69714f, 0.0f }, {-1.44446f, 0.0f } };

		for (auto &i : var_data) {
			set_original_global_opt(i.data());
		}
		m_optima = m_original_optima;
		m_initialized = true;
	}
	void valleys::evaluate_objective(real *x, std::vector<real> &obj) {
		real s;

		s = sin(2 * x[0] - 0.5*OFEC_PI) + 3 * cos(x[1]) + 0.5*x[0];
		obj[0] = s;

	}
	
}