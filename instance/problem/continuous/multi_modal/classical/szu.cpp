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

#include "szu.h"
//* 
//* F(vec3{X})=\sum_{i=1}^{D}{-x_i^2}
//*
namespace OFEC {
	
	szu::szu(param_map &v) :
		szu((v.at("proName")), (v.at("numDim")), 1) {
		
	}
	szu::szu(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
		function(name, size_var, size_obj) {
		
	}

	void szu::initialize() {
		set_range(-5.0, 5.0);
		set_init_range(-5.0, 5.0);
		
		m_objective_monitor = true;
		
		std::vector<std::vector<real>> gobj;

		if (m_variable_size == 2) {
			gobj.push_back(std::vector<real>(1, -156.66));
		}
		else if (m_variable_size == 3) {
			gobj.push_back(std::vector<real>(1, -235.0));
		}
		else if (m_variable_size == 4) {
			gobj.push_back(std::vector<real>(1, -313.33));
		}
		else if (m_variable_size == 5) {
			gobj.push_back(std::vector<real>(1, -391.66));
		}
		else if (m_variable_size == 6) {
			gobj.push_back(std::vector<real>(1, -469.99));
		}
		else if (m_variable_size == 7) {
			gobj.push_back(std::vector<real>(1, -548.33));
		}
		else if (m_variable_size == 8) {
			gobj.push_back(std::vector<real>(1, -626.66));
		}
		else if (m_variable_size == 9) {
			gobj.push_back(std::vector<real>(1, -704.99));
		}

		if (m_variable_size >= 2 && m_variable_size <= 9) {
			m_original_optima.append(gobj[0]);
		}
		else {
		}
		m_optima = m_original_optima;
		m_objective_accuracy = 0.1;
		m_variable_accuracy = 1.e-2;
		//setObjSet();
		m_initialized = true;
	}
	void szu::evaluate_objective(real *x, std::vector<real> &obj) {
		real s = 0;
		size_t i;

		for (i = 0; i < m_variable_size; ++i) {
			s += pow(x[i], 4) - 16 * x[i] * x[i] + 5 * x[i];
		}
		obj[0] = s;

	}
	
}