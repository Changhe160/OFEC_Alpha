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
*  Paper: A sequential niching memetic algorithm for continuous multimodal
*		  Appled Mathematics and Computation 218(2012) 8242-8259
*******************************************************************************************/
// Created: 21 July 2011
// Last modified:
#include "FIBA.h"
namespace OFEC {
	
	FIBA::FIBA(param_map &v) :
		FIBA((v.at("proName")), 2, 1) {
		
		
	}
	FIBA::FIBA(const std::string &name, size_t size_var, size_t size_obj) : problem(name, size_var, size_obj), \
		function(name, size_var, size_obj), m_case(1) {
		
	}
	void FIBA::set_para() {
		if (m_case == 1) {
			m_kappa = -0.95;
			m_chi = -1.26;
		}
		else {
			m_kappa = 4;
			m_chi = 2;
		}
	}
	void FIBA::set_case(int c) {
		m_case = c;
		set_para();
	}

	void FIBA::initialize() {
		set_range(-4.0, 4.0);
		set_init_range(-4., 4.);
		set_para();

		m_variable_monitor = true;
		m_variable_accuracy = 1.e-6;


		if (m_case == 1) {
			m_objective_accuracy = 0.08;
			std::vector<std::vector<real>> var_data = { { 0.45186f, 0.0f },{ -0.22593f, 0.39132f },{ -0.22593f, -0.39132f },{ 0.0f, 0.0f } };
			for (auto &i : var_data) {
				set_original_global_opt(i.data());
			}
		}
		else {
			m_objective_accuracy = 0.5;
			std::vector<std::vector<real>> var_data = { { 0.0f, 0.0f },{ 1.2243f, 0.0f },{ -0.61215f, 1.0603f },{ -0.61215f, -1.0603f } };
			for (auto &i : var_data) {
				set_original_global_opt(i.data());
			}
		}

		m_optima = m_original_optima;
		m_initialized = true;
	}
	void FIBA::evaluate_objective(real *x, std::vector<real> &obj) {

		real s = 0;
		real t0 = x[0] * x[0] + x[1] * x[1];
		s = (t0) / (1 + t0) + m_kappa*(14 * t0 + pow(t0, 2)*m_chi*m_chi - 2 * sqrt(14.)*(pow(x[0], 3) - 3 * x[0] * x[1] * x[1])*m_chi) / (14 * (pow(1 + t0, 2)));
		obj[0] = s + m_bias;

	}
	
}