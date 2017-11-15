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

#include "F10_global_composition2.h"
#include "../../global/CEC2015/F6_hybrid1.h"
#include "../../global/CEC2015/F7_hybrid2.h"
#include "../../global/CEC2015/F8_hybrid3.h"


namespace OFEC {
	namespace CEC2015 {
		F10_global_composition2::F10_global_composition2(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
			composition_2015((v[param_proName]), (v[param_numDim]), 1) {
			m_num_function = 3;
			m_hybrid.resize(m_num_function);
			m_height.resize(m_num_function);
			m_converge_severity.resize(m_num_function);
			m_f_bias.resize(m_num_function);
			initialize();
		}
		F10_global_composition2::F10_global_composition2(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			composition_2015(name, size_var, size_obj) {
			m_num_function = 3;
			m_hybrid.resize(m_num_function);
			m_height.resize(m_num_function);
			m_converge_severity.resize(m_num_function);
			m_f_bias.resize(m_num_function);
			initialize();
		}
		F10_global_composition2::~F10_global_composition2() {
			if (m_num_function > 0) {
				for (auto &i : m_hybrid)
					if (i) delete i;
			}
		}
		void F10_global_composition2::set_function() {
			basic_func f(3);
			f[0] = &create_function<F6_hybrid1>;
			f[1] = &create_function<F7_hybrid2>;
			f[2] = &create_function<F8_hybrid3>;

			for (size_t i = 0; i < m_num_function; ++i) {
				m_hybrid[i] = dynamic_cast<hybrid*>(f[i]("", m_variable_size, m_objective_size));
			}

			m_converge_severity[0] = 10;
			m_converge_severity[1] = 30;
			m_converge_severity[2] = 50;

			m_height[0] = 1;
			m_height[1] = 1;
			m_height[2] = 1;

			m_f_bias[0] = 0;
			m_f_bias[1] = 100;
			m_f_bias[2] = 200;

		}
		void F10_global_composition2::initialize() {
			std::vector<real> hy_bias = { 600,700,800 };
			set_function();
			load_translation("instance/problem/continuous/global/CEC2015/data/");  //data path
			variable<real> temp_var(m_variable_size);
			objective<real> temp_obj(m_objective_size);
			size_t num = 0;
			for (auto &i : m_hybrid) {
				for (size_t j = 0; j < m_variable_size; ++j)
					temp_var[j] = i->get_optima().variable(0)[j] + i->hybrid_translation()[j];
				i->get_optima().clear();
				solution<variable<real>, real> x(temp_var, temp_obj);
				i->get_optima().append(x.get_variable());
				i->evaluate_(x, caller::Problem, false, false);
				x.get_objective()[0] -= hy_bias[num++];
				i->get_optima().append(x.get_objective());
			}
			// Set optimal solution
			for (size_t j = 0; j < m_variable_size; ++j)
				temp_var[j] = m_hybrid[0]->get_optima().variable(0)[j];

			solution<variable<real>, real> x(std::move(temp_var), std::move(temp_obj));

			m_optima.append(x.get_variable());
			evaluate_(x, caller::Problem, false, false);
			m_optima.append(x.get_objective());
			
		}

		void F10_global_composition2::evaluate__(real *x, std::vector<real>& obj) {
			std::vector<real> x_(m_variable_size);
			std::copy(x, x + m_variable_size, x_.begin());
			std::vector<double> weight(m_num_function, 0);

			set_weight(weight, x_);
			std::vector<real> fit(m_num_function);
			variable<real> temp_var(m_variable_size);
			objective<real> temp_obj(m_objective_size);
			solution<variable<real>, real> s(std::move(temp_var), std::move(temp_obj));
			std::vector<real> hy_bias = { 600,700,800 };
			for (size_t i = 0; i < m_num_function; ++i) { // calculate objective value for each function
				s.get_variable() = x_;
				for (size_t j = 0; j < m_variable_size; ++j)
					s.get_variable()[j] -= m_hybrid[i]->hybrid_translation()[j];
				m_hybrid[i]->evaluate_(s, caller::Problem, false, false);
				fit[i] = s.get_objective()[0] - hy_bias[i];

			}
			double sumw = 0;
			for (size_t i = 0; i < m_num_function; ++i)
				sumw += weight[i];
			for (size_t i = 0; i < m_num_function; ++i)
				weight[i] /= sumw;

			double temp = 0;
			for (size_t i = 0; i < m_num_function; ++i) {
				temp += weight[i] * (m_height[i] * fit[i] + m_f_bias[i]);
			}

			obj[0] = temp;
			obj[0] += 1000;
		}
		bool F10_global_composition2::load_translation(const std::string &path) {
			std::string s;
			std::stringstream ss;
			ss << m_variable_size << "Dim.txt";
			s = ss.str();
			s.insert(0, m_name + "_Shift_");
			s.insert(0, path);    // data path
			s.insert(0, global::ms_arg[param_workingDir]);

			for (auto &i : m_hybrid)
				i->hybrid_translation().resize(m_variable_size);
			std::ifstream in(s);
			if (in.fail()) {
				set_translation();
				std::ofstream out(s);
				for (size_t i = 0; i < m_num_function; ++i)
					for (size_t j = 0; j < m_variable_size; ++j)
						out << m_hybrid[i]->hybrid_translation()[j] << " ";

				out.close();
			}
			else {
				for (size_t i = 0; i < m_num_function; ++i)
					for (size_t j = 0; j < m_variable_size; ++j)
						in >> m_hybrid[i]->hybrid_translation()[j];
			}
			in.close();
			return true;
		}

		void F10_global_composition2::set_translation() {
			for (int i = 0; i < m_num_function; i++)
				for (int j = 0; j < m_variable_size; j++)
					m_hybrid[i]->hybrid_translation()[j] = m_domain[j].limit.first + (m_domain[j].limit.second - m_domain[j].limit.first)*(1 - global::ms_global->m_uniform[caller::Problem]->next());
		}
		void F10_global_composition2::set_weight(std::vector<double>& weight, const std::vector<real>&x) {

			for (size_t i = 0; i < m_num_function; ++i) { // calculate weight for each function
				weight[i] = 0;
				for (size_t j = 0; j < m_variable_size; ++j) {
					//weight[i] += pow(x[j] - m_function[i]->translation()[j], 2);
					weight[i] += pow(x[j] - m_hybrid[i]->get_optima().variable(0)[j], 2);
				}

				if (fabs(weight[i])>1e-6) weight[i] = exp(-weight[i] / (2 * m_variable_size*m_converge_severity[i] * m_converge_severity[i])) / (double)(pow(weight[i], 0.5));
				else {
					for (auto &m : weight) {
						m = 0;
					}
					weight[i] = 1;
					break;
				}
			}
		}
		hybrid* F10_global_composition2::get_hybrid(size_t num){
			return m_hybrid[num]; 
		}
	}
}


