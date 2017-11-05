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

#include "F17_hybrid_composition_noisy.h"
#include "../classical/sphere.h"
#include "../classical/ackley.h"
#include "../classical/griewank.h"
#include "../classical/rastrigin.h"
#include "../classical/weierstrass.h"

namespace OFEC {
	namespace CEC2005 {
		F17_hybrid_composition_noisy::F17_hybrid_composition_noisy(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
			composition((v[param_proName]), (v[param_numDim]), 1) {

			initialize();
		}
		F17_hybrid_composition_noisy::F17_hybrid_composition_noisy(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			composition(name, size_var, size_obj) {

			initialize();
		}

		void F17_hybrid_composition_noisy::initialize() {
			
			set_function();
			load_rotation("instance/problem/continuous/global/classical/CEC2005/data/");
			
			compute_fmax();

			load_translation("instance/problem/continuous/global/classical/CEC2005/data/");  //data path
			
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

		void F17_hybrid_composition_noisy::evaluate__(real *x, std::vector<real>& obj) {
			composition::evaluate__(x, obj);
			obj[0] -= 120.;
			obj[0] = obj[0] * noise() + 120.;      

		}
		double F17_hybrid_composition_noisy::noise() {
			return (1 + 0.2*fabs(global::ms_global->m_normal[caller::Problem]->next()));
		}

		void F17_hybrid_composition_noisy::set_function() {
			basic_func f(5);
			f[0] = &create_function<rastrigin>;
			f[1] = &create_function<weierstrass>;
			f[2] = &create_function<griewank>;
			f[3] = &create_function<ackley>;
			f[4] = &create_function<sphere>;

			for (size_t i = 0; i < m_num_function; ++i) {
				m_function[i] = dynamic_cast<function*>(f[i / 2]("", m_variable_size, m_objective_size));
				m_function[i]->set_bias(0);
			}

			for (auto &i : m_function)
				i->set_condition_number(2.);

			for (int i = 0; i < m_num_function; i++) {
				m_height[i] = 100 * i;
				m_converge_severity[i] = 1.;
			}

			m_function[0]->set_range(-5, 5);     m_function[1]->set_range(-5, 5);
			m_function[2]->set_range(-0.5, 0.5); m_function[3]->set_range(-0.5, 0.5);
			m_function[4]->set_range(-60, 60); m_function[5]->set_range(-60, 60);
			m_function[6]->set_range(-32, 32);   m_function[7]->set_range(-32, 32);
			m_function[8]->set_range(-100, 100); m_function[9]->set_range(-100, 100);


			m_stretch_severity[0] = 1.;		m_stretch_severity[1] = 1.;
			m_stretch_severity[2] = 10.;		m_stretch_severity[3] = 10.;
			m_stretch_severity[4] = 5. / 60;  m_stretch_severity[5] = 5. / 60;
			m_stretch_severity[6] = 5. / 32;	m_stretch_severity[7] = 5. / 32;
			m_stretch_severity[8] = 5. / 100;  m_stretch_severity[9] = 5. / 100;

			for (int i = 0; i < m_num_function; i++) {
				m_function[i]->set_scale(m_stretch_severity[i]);
			}

			//set_bias(120.);
		}
		
	}
}


