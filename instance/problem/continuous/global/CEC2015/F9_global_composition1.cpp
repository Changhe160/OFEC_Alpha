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

#include "F9_global_composition1.h"
#include "../../global/classical/schwefel.h"
#include "../../global/classical/rastrigin.h"
#include "../../global/classical/HGBat.h"


namespace OFEC {
	namespace CEC2015 {
		F9_global_composition1::F9_global_composition1(param_map &v) :
			F9_global_composition1((v.at("proName")), (v.at("numDim")), 1) {
			
		}
		F9_global_composition1::F9_global_composition1(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			composition_2015(name, size_var, size_obj) {
			
		}

		void F9_global_composition1::set_function() {
			basic_func f(3);
			f[0] = &create_function<schwefel>;
			f[1] = &create_function<rastrigin>;
			f[2] = &create_function<HGBat>;

			for (size_t i = 0; i < m_num_function; ++i) {
				m_function[i] = dynamic_cast<function*>(f[i]("", m_variable_size, m_objective_size));
				m_function[i]->initialize();
				m_function[i]->set_bias(0);
			}

			for (auto &i : m_function)
				i->set_condition_number(2.);

			m_converge_severity[0] = 20;
			m_converge_severity[1] = 20;
			m_converge_severity[2] = 20;

			m_height[0] = 1;
			m_height[1] = 1;
			m_height[2] = 1;

			m_f_bias[0] = 0;
			m_f_bias[1] = 100;
			m_f_bias[2] = 200;

		}
		void F9_global_composition1::initialize() {
			m_variable_monitor = true;
			m_num_function = 3;
			m_function.resize(m_num_function);
			m_height.resize(m_num_function);
			m_converge_severity.resize(m_num_function);
			m_f_bias.resize(m_num_function);
			set_function();
			load_translation("instance/problem/continuous/global/CEC2015/data/");  //data path
			load_rotation("instance/problem/continuous/global/CEC2015/data/");
			for (auto &i : m_function) {
				i->get_optima().clear();
				i->set_global_opt(i->translation().data());
			}
			// Set optimal solution
            solution<variable_vector<real>, real> s(m_objective_size, num_constraints(), m_variable_size);
			s.variable() = m_function[0]->get_optima().variable(0);
			m_optima.append(s.variable());
			s.evaluate(false, caller::Problem);
			m_optima.append(s.objective());

			m_initialized = true;
		}

		void F9_global_composition1::evaluate_objective(real *x, std::vector<real> &obj) {
			std::vector<real> x_(m_variable_size);
			std::copy(x, x + m_variable_size, x_.begin());
			std::vector<real> weight(m_num_function, 0);

			set_weight(weight, x_);
			std::vector<real> fit(m_num_function);
			variable_vector<real> temp_var(m_variable_size);
			std::vector<real> temp_obj(m_objective_size);
            solution<variable_vector<real>, real> s(m_objective_size, num_constraints(), m_variable_size);

			for (size_t i = 0; i < m_num_function; ++i) { // calculate objective value for each function
				s.variable().vect() = x_;
				if (i == 0) m_function[i]->set_rotation_flag(false);
				m_function[i]->evaluate_(s, caller::Problem, false, false);
				fit[i] = s.objective()[0];

			}
			real sumw = 0;
			for (size_t i = 0; i < m_num_function; ++i)
				sumw += weight[i];
			for (size_t i = 0; i < m_num_function; ++i)
				weight[i] /= sumw;

			real temp = 0;
			for (size_t i = 0; i < m_num_function; ++i) {
				temp += weight[i] * (m_height[i] * fit[i] + m_f_bias[i]);
			}

			obj[0] = temp;
			obj[0] += 900;
		}

	}
}


