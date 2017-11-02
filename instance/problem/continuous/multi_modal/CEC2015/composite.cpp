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


#include "composite.h"
namespace OFEC {
	namespace CEC2015 {
		composite::composite(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), function(name, size_var, size_obj), m_num_function(5), \
			m_function(m_num_function), m_height(m_num_function), m_f_bias(m_num_function), \
			m_converge_severity(m_num_function) {

			set_range(-100., 100.);
		}
		composite::~composite() {
			//dtor
			if (m_num_function > 0) {
				for (auto &i : m_function)
					if (i) delete i;
			}
		}

		size_t composite::get_num_function() {
			return m_num_function;
		}

		void composite::set_weight(vector<double>& weight, const vector<real>&x) { //default CEC05
			
			for (size_t i = 0; i < m_num_function; ++i) { // calculate weight for each function
				weight[i] = 0;
				for (size_t j = 0; j < m_variable_size; ++j) {
					weight[i] += pow(x[j] - m_function[0]->get_optima().variable(0)[j],2);
				}
				
				if(fabs(weight[i])>1e-6) weight[i] = exp(-weight[i] / (2 * m_variable_size*m_converge_severity[i] * m_converge_severity[i])) / (double)(pow(weight[i], 0.5));
				else {
					for (auto &m : weight) {
						m = 0;
					}
					weight[i] = 1;
					break;
				}
			}
		}

		void composite::evaluate__(real *x, std::vector<real>& obj) {
			std::vector<real> x_(m_variable_size);
			std::copy(x, x + m_variable_size, x_.begin());
			std::vector<double> weight(m_num_function, 0);

			set_weight(weight, x_);
			std::vector<real> fit(m_num_function);
			variable<real> temp_var(m_variable_size);
			objective<real> temp_obj(m_objective_size);
			solution<variable<real>, real> s(std::move(temp_var), std::move(temp_obj));
			s.get_variable() = x_;
			for (size_t i = 0; i < m_num_function; ++i) { // calculate objective value for each function

				m_function[i]->evaluate_(s, caller::Problem, false, false);
				fit[i] = s.get_objective()[0];
				
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

			obj[0] = temp + m_bias;
		}
		
		function* composite::get_function(size_t num) {
			return m_function[num];
		}
		bool composite::load_translation(const string &path) {
			string s;
			stringstream ss;
			ss << m_variable_size << "Dim.txt";
			s = ss.str();
			s.insert(0, m_name + "_Opt_");
			s.insert(0, path);    // data path
			s.insert(0, global::ms_arg[param_workingDir]);

			ifstream in(s);
			if (in.fail()) {
				return false;
			}
			else {
				for (size_t i = 0; i < m_num_function; ++i) {
					for (size_t j = 0; j < m_variable_size; ++j) {
						in >> m_function[i]->translation()[j];
						m_function[i]->set_tranlation_flag(true);
					}
				}
			}
			in.close();
			//m_translation_flag = true;
			return true;
		}
		void composite::set_translation() {
			for (int i = 0; i < m_num_function; i++) {
				m_function[i]->translation().resize(m_variable_size);
				for (int j = 0; j < m_variable_size; j++) {
					m_function[i]->translation()[j] = m_domain[j].limit.first + (m_domain[j].limit.second - m_domain[j].limit.first)*(1 - global::ms_global->m_uniform[caller::Problem]->next());
				}
				m_function[i]->set_tranlation_flag(true);
			}
		}
		bool composite::load_rotation(const string &path) {
			string s;
			stringstream ss;
			ss << m_variable_size << "Dim.txt";
			s = ss.str();
			s.insert(0, m_name + "_RotM_");

			s.insert(0, path);// data path
			s.insert(0, global::ms_arg[param_workingDir]);

			ifstream in(s);
			if (in.fail()) {
				return false;
			}
			else {
				for (size_t i = 0; i < m_num_function; ++i) {
					for (size_t j = 0; j < m_variable_size; ++j) {
						m_function[i]->rotation().load(in);
						m_function[i]->set_rotation_flag(true);
					}
				}
			}
			in.close();
			//m_rotation_flag = true;
			return true;
		}
		void composite::set_rotation() {
			for (auto i : m_function) {
				i->rotation().generate_rotation_classical(global::ms_global->m_normal[caller::Problem].get(), m_condition_number);
				i->set_rotation_flag(true);
			}
		}
	}
	
}

