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

#include "max_global.h"

namespace OFEC {
	
	max_global::max_global(param_map &v) : problem((v.at("proName")),1, 1), \
		function((v.at("proName")), 1, 1) {
		
		
	}
	max_global::max_global(const std::string &name, size_t size_var, size_t size_obj) : problem(name, size_var, size_obj), \
		function(name, size_var, size_obj) {
		
	}

	void max_global::initialize_problem() {
		set_range(0, 1);
		set_init_range(0, 1);
		m_opt_mode[0] = optimization_mode::Maximization;
		m_variable_monitor = true;
		 //5 gopt

		m_objective_accuracy = 0.1;
		m_variable_accuracy = 1.e-5;
		std::vector<std::vector<real>> var_data = { { 0.5 },{ 0.1 },{ 0.3 },{ 0.7 },{0.9} };
		for (auto &i : var_data) {
			set_original_global_opt(i.data());
		}

		m_optima = m_original_optima;
		set_tag(std::set<problem_tag>({ problem_tag::MMP, problem_tag::CONT }));
	}

	void max_global::evaluate__(real *x, std::vector<real>& obj) {

		obj[0] = pow(sin(5 * OFEC_PI*x[0]), 6.) + m_bias;

	}
	
}