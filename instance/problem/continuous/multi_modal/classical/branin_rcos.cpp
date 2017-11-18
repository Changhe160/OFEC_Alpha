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


#include "branin_rcos.h"

namespace OFEC {

	branin_rcos::branin_rcos(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
		function((v[param_proName]), (v[param_numDim]), 1) {
		v[param_numDim] = 2;

		std::vector<std::pair<real, real>> range;
		range.push_back(std::make_pair(-5., 10.));
		range.push_back(std::make_pair(0., 15.));
		set_init_range(range);
		set_range(std::move(range));
		initialize();
	}
	branin_rcos::branin_rcos(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
		function(name, size_var, size_obj) {
		std::vector<std::pair<real, real>> range;
		range.push_back(std::make_pair(-5., 10.));
		range.push_back(std::make_pair(0., 15.));
		set_init_range(range);
		set_range(std::move(range));
		initialize();
	}

	void branin_rcos::initialize() {
		m_opt_mode[0] = optimization_mode::Minimization;
		m_variable_accuracy = 1.0;
		m_objective_accuracy = 1.e-5;

		m_original_optima.set_number_variable(3);

		std::vector<std::vector<real>> var_data = { { -OFEC_PI, 12.275 },{ -OFEC_PI, 2.275 },{9.42478, 2.475} };
		for (auto &i : var_data) {
			set_original_global_opt(i.data());
		}
		m_optima = m_original_optima;
		add_tag(problem_tag::MMP);
		//setObjSet();
	}
	void branin_rcos::evaluate__(real *x, std::vector<real>& obj) {

		double s, a;
		a = x[1] - 5.1*x[0] * x[0] / (4 * OFEC_PI*OFEC_PI) + 5 * x[0] / OFEC_PI - 6;
		s = a*a + 10 * (1 - 1 / (8 * OFEC_PI))*cos(x[0]) + 10;
		obj[0] = s + m_bias;

	}
	
}