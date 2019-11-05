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
*  Paper; A sequential niching memetic algorithm for continuous multimodal
*		  Appled Mathematics and Computation 218(2012) 8242-8259
*******************************************************************************************/
/*******************************************************************************************
*  Paper: A sequential niching memetic algorithm for continuous multimodal
*		  Appled Mathematics and Computation 218(2012) 8242-8259
****************************************************************************************
*  LaTex:F(x)=\frac{1}{D}\sum^D_{i=1}{sin(10\log(x_i))}
*******************************************************************************************/

#include "vincent.h"
namespace OFEC {
	
	vincent::vincent(param_map &v) : vincent((v.at("proName")), (v.at("numDim"))) {}

	vincent::vincent(const std::string &name, size_t size_var) : problem(name, size_var, 1), function(name, size_var, 1) {}

	void vincent::initialize() { // note
		set_range(0.25, 10.); // note
		set_init_range(0.25, 10.);
		m_opt_mode[0] = optimization_mode::Maximization;  // note
		m_objective_monitor = true;
		if (global::ms_arg.find("objectiveAccuracy") == global::ms_arg.end())
			global::ms_arg.insert({ "objectiveAccuracy",(real)1.e-4 });
		m_objective_accuracy = global::ms_arg.at("objectiveAccuracy");
		m_variable_accuracy = 0.2;

		size_t num_solution = pow(6, m_variable_size);
		
		std::vector<std::vector<real>> obj_data((int)num_solution, std::vector<real>(m_objective_size, 1));
		for (auto &i : obj_data)
			m_original_optima.append(i);
		m_optima = m_original_optima;

		m_variable_partition.resize(m_variable_size);
		for (size_t i = 0; i < m_variable_size; ++i)
			m_variable_partition[i] = { i };
		m_initialized = true;
	}
	void vincent::evaluate_objective(real *x, std::vector<real> &obj) {
		real s = 0;

		for (int i = 0; i < m_variable_size; ++i) {
			s += sin(10 * log(x[i]));
		}
		s /= m_variable_size;
		obj[0] = s + m_bias;  // note
	}
	
}