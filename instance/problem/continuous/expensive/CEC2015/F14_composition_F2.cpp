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

#include "F14_composition_F2.h"
#include "../../global/classical/schwefel.h"
#include "../../global/classical/rastrigin.h"
#include "../../global/classical/elliptic.h"



namespace OFEC {
	namespace CEC2015 {
		F14_composition_F2::F14_composition_F2(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
			composition_2015((v[param_proName]), (v[param_numDim]), 1) {
			m_num_function = 3;
			m_function.resize(m_num_function);
			m_height.resize(m_num_function);
			m_f_bias.resize(m_num_function);
			m_converge_severity.resize(m_num_function);
			initialize();
		}
		F14_composition_F2::F14_composition_F2(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			composition_2015(name, size_var, size_obj) {
			m_num_function = 3;
			m_function.resize(m_num_function);
			m_height.resize(m_num_function);
			m_f_bias.resize(m_num_function);
			m_converge_severity.resize(m_num_function);
			initialize();
		}
		
		void F14_composition_F2::set_function() {
			basic_func f(3);
			f[0] = &create_function<schwefel>;
			f[1] = &create_function<rastrigin>;
			f[2] = &create_function<elliptic>;
			
			for (size_t i = 0; i < m_num_function; ++i) {
				m_function[i] = dynamic_cast<function*>(f[i]("", m_variable_size, m_objective_size));
				m_function[i]->set_bias(0);
			}

			for (auto &i : m_function)
				i->set_condition_number(2.);

			m_converge_severity[0] = 10;
			m_converge_severity[1] = 30;
			m_converge_severity[2] = 50;

			m_height[0] = 0.25;
			m_height[1] = 1;
			m_height[2] = 1e-7;

			m_f_bias[0] = 0;
			m_f_bias[1] = 100;
			m_f_bias[2] = 200;

			//set_bias(1400);
		}
		void F14_composition_F2::initialize() {
			set_function();
			load_translation("instance/problem/continuous/expensive/CEC2015/data/");  //data path
			
			load_rotation("instance/problem/continuous/expensive/CEC2015/data/");  //data path
			
			for (auto &i : m_function) {
				i->get_optima().clear();
				i->set_global_opt(i->translation().data());
			}
			// Set optimal solution
			variable<real> temp_var(m_variable_size);
			objective<real> temp_obj(m_objective_size);
			solution<variable<real>, real> x(std::move(temp_var), std::move(temp_obj));
			for (int i = 0; i < m_variable_size; ++i) {
				x.get_variable()[i] = m_function[0]->get_optima().variable(0)[i];
			}
			m_optima.append(x.get_variable());

			evaluate_(x, caller::Problem, false, false);
			m_optima.append(x.get_objective());
		}

		void F14_composition_F2::evaluate__(real *x, std::vector<real>& obj) {
			composition_2015::evaluate__(x, obj);
			obj[0] += 1400;
		}

	}
}


