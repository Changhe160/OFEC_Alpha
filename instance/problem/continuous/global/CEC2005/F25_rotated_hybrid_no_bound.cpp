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

#include "F25_rotated_hybrid_no_bound.h"


namespace OFEC {
	namespace CEC2005 {
		F25_rotated_hybrid_no_bound::F25_rotated_hybrid_no_bound(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
			F24_rotated_hybrid_composition((v[param_proName]), (v[param_numDim]), 1) {

			initialize();
		}
		F25_rotated_hybrid_no_bound::F25_rotated_hybrid_no_bound(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			F24_rotated_hybrid_composition(name, size_var, size_obj) {

			initialize();
		}
		F25_rotated_hybrid_no_bound::~F25_rotated_hybrid_no_bound() {
			//dtor


		}

		void F25_rotated_hybrid_no_bound::initialize() {

			set_range(2, 5);
			for (size_t i = 0; i < m_variable_size; ++i)
				m_domain[i].limited = false;
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

		}

		void F25_rotated_hybrid_no_bound::evaluate__(real *x, std::vector<real>& obj) {
			F24_rotated_hybrid_composition::evaluate__(x, obj);
		}
		void F25_rotated_hybrid_no_bound::set_translation() {
			for (size_t i = 0; i < m_num_function; ++i) {
				m_function[i]->translation().resize(m_variable_size);
				m_function[i]->set_tranlation_flag(true);
			}
			for (size_t j = 0; j < m_variable_size; ++j) {
				m_function[0]->translation()[j] = -5.0 + (7.0)*(global::ms_global->m_uniform[caller::Problem]->next());
			}

			for (size_t i = 1; i < m_num_function - 1; ++i)
				for (int j = 0; j < m_variable_size; ++j) {
					m_function[i]->translation()[j] = m_domain[j].limit.first + (m_domain[j].limit.second - m_domain[j].limit.first)*(1 - global::ms_global->m_uniform[caller::Problem]->next());
				}

			for (size_t j = 0; j < m_variable_size; ++j) {
				m_function[m_num_function - 1]->translation()[j] = 0;
			}

		}
	}
}


