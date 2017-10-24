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

#include "F23_non_continuous_rotated_hybrid.h"




namespace OFEC {
	namespace CEC2005 {
		F23_non_continuous_rotated_hybrid::F23_non_continuous_rotated_hybrid(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
			F21_rotated_hybrid_composition((v[param_proName]), (v[param_numDim]), 1) {

			initialize();
		}
		F23_non_continuous_rotated_hybrid::F23_non_continuous_rotated_hybrid(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			F21_rotated_hybrid_composition(name, size_var, size_obj) {

			initialize();
		}
		F23_non_continuous_rotated_hybrid::~F23_non_continuous_rotated_hybrid() {
			//dtor


		}

		void F23_non_continuous_rotated_hybrid::initialize() {


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

		void F23_non_continuous_rotated_hybrid::evaluate__(real *x, std::vector<real>& obj) {
			std::vector<real> x_(m_variable_size);
			for (size_t j = 0; j < m_variable_size; ++j) {
				real trans = m_function[0]->translation()[j];
				if (fabs(x[j] - trans) >= 0.5) {
					real temp_x = 2.0 * x[j];
					int int_part = int(temp_x);
					double decimal_part = temp_x - int_part;
					if (temp_x <= 0 && decimal_part >= 0.5) temp_x = int_part - 1;
					else if (decimal_part < 0.5) temp_x = int_part;
					else temp_x = int_part + 1;
					x_[j] = temp_x / 2.0;
				}
				else x_[j] = x[j];
			}
			vector<double> weight(m_num_function, 0);

			for (size_t i = 0; i < m_num_function; ++i) { // calculate weight for each function
				weight[i] = 0;
				for (size_t j = 0; j < m_variable_size; ++j) {
					weight[i] += (x[j] - m_function[i]->translation()[j])*(x[j] - m_function[i]->translation()[j]);
				}
				weight[i] = exp(-weight[i] / (2 * m_variable_size*m_converge_severity[i] * m_converge_severity[i]));

			}
			vector<real> fit(m_num_function);
			variable<real> temp_var(m_variable_size);
			objective<real> temp_obj(m_objective_size);
			solution<variable<real>, real> s(std::move(temp_var), std::move(temp_obj));
			s.get_variable() = x_;
			for (size_t i = 0; i < m_num_function; ++i) { // calculate objective value for each function

				m_function[i]->function::evaluate_(s, caller::Problem, false, false);
				fit[i] = s.get_objective()[0];
				if (fabs(m_fmax[i]) > 1e-6)
					fit[i] = m_height_normalize_severity*fit[i] / fabs(m_fmax[i]);
			}

			double sumw = 0, wmax;
			wmax = *max_element(weight.begin(), weight.end());
			for (size_t i = 0; i < m_num_function; ++i) {
				if (weight[i] != wmax) {
					weight[i] = weight[i] * (1 - pow(wmax, 10));
				}
			}
			size_t same_wmax_num = 0;
			for (size_t i = 0; i < m_num_function; ++i) {
				if (weight[i] == wmax) ++same_wmax_num;
			}
			size_t i = m_num_function - 1;
			while (same_wmax_num > 1 && i >= 0) {
				if (wmax == weight[i]) {
					weight[i] = 0;
					--same_wmax_num;
				}
				--i;
			}

			for (size_t i = 0; i < m_num_function; ++i)
				sumw += weight[i];
			for (size_t i = 0; i < m_num_function; ++i)
				weight[i] /= sumw;

			double temp = 0;
			for (size_t i = 0; i < m_num_function; ++i) {
				temp += weight[i] * (fit[i] + m_height[i]);
			}

			obj[0] = temp + m_bias;
		}
	}
}


