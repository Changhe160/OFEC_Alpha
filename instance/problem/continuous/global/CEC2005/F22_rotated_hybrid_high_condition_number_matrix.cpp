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

#include "F22_rotated_hybrid_high_condition_number_matrix.h"


namespace OFEC {
	namespace CEC2005 {
		F22_rotated_hybrid_high_condition_number_matrix::F22_rotated_hybrid_high_condition_number_matrix(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
			F21_rotated_hybrid_composition((v[param_proName]), (v[param_numDim]), 1) {

			initialize();
		}
		F22_rotated_hybrid_high_condition_number_matrix::F22_rotated_hybrid_high_condition_number_matrix(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			F21_rotated_hybrid_composition(name, size_var, size_obj) {

			initialize();
		}
		F22_rotated_hybrid_high_condition_number_matrix::~F22_rotated_hybrid_high_condition_number_matrix() {
			//dtor


		}

		void F22_rotated_hybrid_high_condition_number_matrix::initialize() {

			m_function[0]->set_condition_number(10.); m_function[1]->set_condition_number(20.);
			m_function[2]->set_condition_number(50.); m_function[3]->set_condition_number(100.);
			m_function[4]->set_condition_number(200.); m_function[5]->set_condition_number(1000.);
			m_function[6]->set_condition_number(2000.); m_function[7]->set_condition_number(3000.);
			m_function[8]->set_condition_number(3000.); m_function[9]->set_condition_number(5000.);

			bool is_load = load_rotation("instance/problem/continuous/global/CEC2005/data/");
			if (!is_load) {
				set_rotation();
			}
			for (auto &i : m_function) {
				i->set_tranlation_flag(false);
			}
			compute_fmax();

			is_load = load_translation("instance/problem/continuous/global/CEC2005/data/");  //data path
			if (!is_load) {
				set_translation();
			}
			for (auto &i : m_function) {
				i->get_optima().clear();
				i->set_global_opt(i->translation().data());
			}

			// Set optimal solution
			m_optima.clear();
			variable<real> temp_var(m_variable_size);
			objective<real> temp_obj(m_objective_size);
			solution<variable<real>, real> x(std::move(temp_var), std::move(temp_obj));
			for (int i = 0; i < m_variable_size; ++i) {
				x.get_variable()[i] = m_function[0]->translation()[i];
			}
			m_optima.append(x.get_variable());

			evaluate_(x, caller::Problem, false, false);
			m_optima.append(x.get_objective());
			// end set
			m_variable_accuracy = 1.0e-3;
			m_objective_accuracy = 0.1;
		}

		void F22_rotated_hybrid_high_condition_number_matrix::evaluate__(real *x, std::vector<real>& obj) {
			F21_rotated_hybrid_composition::evaluate__(x, obj);
		}
	}
}


