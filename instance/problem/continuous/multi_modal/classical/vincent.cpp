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
	
	vincent::vincent(param_map &v) : problem((v[param_proName]), (v[param_numDim]), 1), \
		function((v[param_proName]), (v[param_numDim]), 1) {

		set_range(0.25, 10.); // note
		initialize();
	}
	vincent::vincent(const std::string &name, size_t size_var, size_t size_obj) : problem(name, size_var, size_obj), \
		function(name, size_var, size_obj) {
		set_range(0.25, 10.); // note
		initialize();
	}

	void vincent::initialize() { // note
		add_tag(problem_tag::MMP);
		m_opt_mode[0] = optimization_mode::Maximization;  // note

		m_objective_accuracy = 0.2;
		m_variable_accuracy = 1.e-4;

		m_original_optima.set_number_variable((int)pow(6, m_variable_size));
		std::vector<std::vector<real>> obj_data((int)pow(6, m_variable_size), std::vector<real>(m_objective_size, 1));
		for (auto &i : obj_data)
			m_original_optima.append(i);
		m_optima = m_original_optima;
		
	}
	void vincent::evaluate__(real *x, std::vector<real>& obj) {
		double s = 0;

		for (int i = 0; i < m_variable_size; ++i) {
			s += sin(10 * log(x[i]));
		}
		s /= m_variable_size;
		obj[0] = s + m_bias;  // note
	}
	
}