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

#include "F13_composition_F1.h"
#include "../../global/classical/rotated_rosenbrock.h"
#include "../../global/classical/elliptic.h"
#include "../../global/classical/rotated_bent_cigar.h"
#include "../../global/classical/rotated_discus.h"
#include "../../global/classical/elliptic.h"





namespace OFEC {
	namespace CEC2015 {
		F13_composition_F1::F13_composition_F1(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
			composition_2015((v[param_proName]), (v[param_numDim]), 1) {

			initialize();
		}
		F13_composition_F1::F13_composition_F1(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			composition_2015(name, size_var, size_obj) {

			initialize();
		}
		
		void F13_composition_F1::set_function() {
			basic_func f(5);
			f[0] = &create_function<rotated_rosenbrock>;
			f[1] = &create_function<elliptic>;
			f[2] = &create_function<rotated_bent_cigar>;
			f[3] = &create_function<rotated_discus>;
			f[4] = &create_function<elliptic>;

			for (size_t i = 0; i < m_num_function; ++i) {
				m_function[i] = dynamic_cast<function*>(f[i]("", m_variable_size, m_objective_size));
				m_function[i]->set_bias(0);
			}

			for (auto &i : m_function)
				i->set_condition_number(2.);

			m_converge_severity[0] = 10;
			m_converge_severity[1] = 20;
			m_converge_severity[2] = 30;
			m_converge_severity[3] = 40;
			m_converge_severity[4] = 50;

			m_height[0] = 1;
			m_height[1] = 1e-6;
			m_height[2] = 1e-26;
			m_height[3] = 1e-6;
			m_height[4] = 1e-6;

			
			m_f_bias[0] = 0;
			m_f_bias[1] = 100;
			m_f_bias[2] = 200;
			m_f_bias[3] = 300;
			m_f_bias[4] = 400;

			//set_bias(1300);
		}
		void F13_composition_F1::initialize() {
			set_function();
			load_translation("instance/problem/continuous/expensive/CEC2015/data/");  //data path
			
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

		void F13_composition_F1::evaluate__(real *x, std::vector<real>& obj) {
			composition_2015::evaluate__(x, obj);
			obj[0] += 1300;
		}

	}
}


