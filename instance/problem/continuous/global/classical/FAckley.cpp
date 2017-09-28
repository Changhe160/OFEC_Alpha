
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

#include "FAckley.h"
namespace OFEC {

	FAckley::FAckley(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
		function((v[param_proName]), (v[param_numDim]), 1) {

		set_range(-32.768, 32.768);
		initialize();
	}
	FAckley::FAckley(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
		function(name, size_var, 1) {
		set_range(-32.768, 32.768);
		initialize();
	}

	FAckley::~FAckley() {
		//dtor
	}

	void FAckley::initialize() {

		set_original_global_opt();
		m_translation.resize(m_variable_size);
		if (m_name == "FUN_Ackley") {
			translate_zero();   // because m_original_optima is zero
		}
		else if(m_name == "FUN_Ackley_Noisy"){
			translate_zero();
			m_noise_flag = true;
		}
		else if (m_name == "FUN_S_Ackley") {
			
			std::vector<real> temp_var(m_variable_size);
			for (size_t i = 0; i < m_variable_size; ++i)
				temp_var[i] = m_original_optima.variable(0)[i];
			set_translation(temp_var);  // update m_optima
			
			m_translation_flag = true;
		}
		else if (m_name == "FUN_R_Ackley") {
			translate_zero();
			set_condition_number(2);
			m_rotation.resize(m_variable_size, m_variable_size);
			set_rotation();
			m_rotation_flag = true;
		}
		else if (m_name == "FUN_RS_Ackley") {
			set_condition_number(100);
			
			std::vector<real> temp_var(m_variable_size);
			for (size_t i = 0; i < m_variable_size; ++i)
				temp_var[i] = m_original_optima.variable(0)[i];
			set_translation(temp_var);  // update m_optima
			
			m_rotation.resize(m_variable_size, m_variable_size);
			set_rotation();
			m_translation_flag = true;
			m_rotation_flag = true;
		}
		else if (m_name == "FUN_CEC05_F08_RS_Ackley_Bound") {
			set_condition_number(100);
			
			std::vector<real> temp_var(m_variable_size);
			for (size_t i = 0; i < m_variable_size; ++i)
				temp_var[i] = m_original_optima.variable(0)[i];
			set_translation(temp_var);  // update m_optima

			m_rotation.resize(m_variable_size, m_variable_size);
			set_rotation();
			set_bias(-140);
			m_translation_flag = true;
			m_rotation_flag = true;
		}
		else if (m_name == "FUN_S_Ackley_CEC08") {
			set_bias(-140);
			
			std::vector<real> temp_var(m_variable_size);
			for (size_t i = 0; i < m_variable_size; ++i)
				temp_var[i] = m_original_optima.variable(0)[i];
			set_translation(temp_var);  // update m_optima
			
			m_translation_flag = true;
		}
		else {
			throw myexcept("Error: please check the problem name@FAckley::initialize()");
		}
		set_global_opt(m_translation.data());
	}

	void FAckley::evaluate__(real *x, vector<real>& obj) {
		real s1 = 0, s2 = 0;
		if (m_translation_flag) 
			translate(x);
		if (m_rotation_flag) 
			rotate(x);
		if (m_scale_flag)
			scale(x);
		if (m_translation_flag)
		    translate2(x);
		if (m_noise_flag) {

			double noise;
			for (int i = 0; i < m_variable_size; i++) {
				noise = 0.01*global::ms_global->m_uniform[caller::Problem]->next();
				s1 += (x[i] + noise)*(x[i] + noise);
				s2 += cos(2 * OFEC_PI*(x[i] + noise));
			}
		}
		else {
			for (int i = 0; i < m_variable_size; i++) {
				s1 += x[i] * x[i];
				s2 += cos(2 * OFEC_PI*x[i]);
			}
		}

		obj[0] = -20 * exp(-0.2*sqrt(s1 / m_variable_size)) - exp(s2 / m_variable_size) + 20 + OFEC_E + m_bias;

	}

}