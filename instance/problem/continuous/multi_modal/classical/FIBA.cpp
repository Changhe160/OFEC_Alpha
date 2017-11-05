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
	
	FIBA::FIBA(param_map &v) : problem((v[param_proName]), (v[param_numDim]), 1), \
		function((v[param_proName]), (v[param_numDim]), 1), m_case(v[param_case] = (v.find(param_case) == v.end() ? 1 : (int)v[param_case])) {
		v[param_numDim] = 2;
		set_range(-4.0, 4.0);
		initialize();
	}
	FIBA::FIBA(const std::string &name, size_t size_var, size_t size_obj) : problem(name, size_var, size_obj), \
		function(name, size_var, size_obj), m_case(1) {
		set_range(-4.0, 4.0);
		initialize();
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
		set_para();

		m_original_optima.set_number_variable(4);
		m_variable_accuracy = 1.e-6;


		if (m_case == 1) {
			m_objective_accuracy = 0.08;
			std::vector<std::vector<real>> var_data = { { 0.45186, 0.0 },{ -0.22593, 0.39132 },{ -0.22593, -0.39132 },{ 0.0, 0.0 } };
			for (auto &i : var_data) {
				set_original_global_opt(i.data());
			}
		}
		else {
			m_objective_accuracy = 0.5;
			std::vector<std::vector<real>> var_data = { { 0.0, 0.0 },{ 1.2243, 0.0 },{ -0.61215, 1.0603 },{ -0.61215, -1.0603 } };
			for (auto &i : var_data) {
				set_original_global_opt(i.data());
			}
		}

		m_optima = m_original_optima;
		add_tag(problem_tag::MMP);
		//setObjSet();
	}
	void FIBA::evaluate__(real *x, std::vector<real>& obj) {

		double s = 0;
		double t0 = x[0] * x[0] + x[1] * x[1];
		s = (t0) / (1 + t0) + m_kappa*(14 * t0 + pow(t0, 2)*m_chi*m_chi - 2 * sqrt(14.)*(pow(x[0], 3) - 3 * x[0] * x[1] * x[1])*m_chi) / (14 * (pow(1 + t0, 2)));
		obj[0] = s + m_bias;

	}
	
}