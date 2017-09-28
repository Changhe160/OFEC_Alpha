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
// Created: 21 July 2011
// Last modified:
#include "FRosenbrock.h"

namespace OFEC {

	FRosenbrock::FRosenbrock(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
		function((v[param_proName]), (v[param_numDim]), 1) {

		set_range(-2.048, 2.048);

		initialize();
	}
	FRosenbrock::FRosenbrock(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
		function(name, size_var, size_obj) {

		set_range(-2.048, 2.048);

		initialize();
	}

	FRosenbrock::~FRosenbrock() {
		//dtor
	}

	void FRosenbrock::initialize() {
		std::vector<double> v(m_variable_size, 1);
		set_original_global_opt(v.data());
		m_translation.resize(m_variable_size);
		if (m_name == "FUN_Rosenbrock") {
			m_translation = v;    // because m_original_optima is vecter "v"
		}
		else if (m_name == "FUN_S_Rosenbrock") {
			std::vector<real> temp_var(m_variable_size);
			for (size_t i = 0; i < m_variable_size; ++i)
				temp_var[i] = m_original_optima.variable(0)[i];
			set_translation(temp_var);  // update m_optima
			m_translation_flag = true;
		}
		else if (m_name == "FUN_CEC05_F06_S_Rosenbrock" || m_name == "FUN_S_Rosenbrock_CEC08") {
			set_bias(390);
			std::vector<real> temp_var(m_variable_size);
			for (size_t i = 0; i < m_variable_size; ++i)
				temp_var[i] = m_original_optima.variable(0)[i];
			set_translation(temp_var);  // update m_optima
			m_translation_flag = true;
		}
		else {
			throw myexcept("Error: please check the problem ID@FRosenbrock::initialize()");
		}
		set_global_opt(m_translation.data());
		//m_objective_accuracy = 1.0e-5;
	}

	void FRosenbrock::evaluate__(real *x, vector<real>& obj) {
		if (m_translation_flag) {
			translate(x);
			translate2(x);
		}
		double fitness = 0;

		for (int i = 0; i < m_variable_size - 1; i++) {
			fitness += 100 * (x[i + 1] - x[i] * x[i])*(x[i + 1] - x[i] * x[i]) + (x[i] - 1)*(x[i] - 1);
		}

		obj[0] = fitness + m_bias;

	}
}