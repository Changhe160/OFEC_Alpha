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


#include "composition.h"
namespace OFEC {
	namespace CEC2005 {
		composition::composition(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), continuous(name, size_var, size_obj), m_num_function(10), \
			m_function(m_num_function), m_height(m_num_function), m_fmax(m_num_function), \
			m_converge_severity(m_num_function), m_stretch_severity(m_num_function) {

			set_range(-5, 5);
			m_height_normalize_severity = 2000.;
		}
		composition::~composition() {
			//dtor
			if (m_num_function > 0) {
				for (auto &i : m_function)
					if (i) delete i;
			}
		}

		size_t composition::get_num_function() {
			return m_num_function;
		}

		void composition::compute_fmax() {  // calculate the estimate max value of funciton i
			variable<real> temp_var(m_variable_size);
			objective<real> temp_obj(m_objective_size);
			solution<variable<real>, real> x(std::move(temp_var), std::move(temp_obj));
			for (size_t i = 0; i < m_num_function; ++i) {
				for (size_t j = 0; j < m_variable_size; ++j) {
					x.get_variable()[j] = m_domain[j].limit.second;
				}
				m_function[i]->function::evaluate_(x, caller::Problem, false, false);
				m_fmax[i] = x.get_objective()[0];
			}
		}

		void composition::set_weight(std::vector<double>& weight, const std::vector<real>&x) { //default CEC05
			for (size_t i = 0; i < m_num_function; ++i) { // calculate weight for each function
				weight[i] = 0;
				for (size_t j = 0; j < m_variable_size; ++j) {
					weight[i] += (x[j] - m_function[i]->translation()[j])*(x[j] - m_function[i]->translation()[j]);
				}
				weight[i] = exp(-weight[i] / (2 * m_variable_size*m_converge_severity[i] * m_converge_severity[i]));

			}
		}

		void composition::evaluate__(real *x, std::vector<real>& obj) {
			std::vector<real> x_(m_variable_size);
			std::copy(x, x + m_variable_size, x_.begin());
			std::vector<double> weight(m_num_function, 0);

			for (size_t i = 0; i < m_num_function; ++i) { // calculate weight for each function
				weight[i] = 0;
				for (size_t j = 0; j < m_variable_size; ++j) {
					weight[i] += (x[j] - m_function[i]->translation()[j])*(x[j] - m_function[i]->translation()[j]);
				}
				weight[i] = exp(-weight[i] / (2 * m_variable_size*m_converge_severity[i] * m_converge_severity[i]));

			}
			std::vector<real> fit(m_num_function);
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

			obj[0] = temp;
		}
		bool composition::load_translation(const std::string &path) {
			std::string s;
			std::stringstream ss;
			ss << m_variable_size << "Dim.txt";
			s = ss.str();
			s.insert(0, m_name + "_Opt_");
			s.insert(0, path);    // data path
			s.insert(0, global::ms_arg[param_workingDir]);

			for (auto &i : m_function)
				i->translation().resize(m_variable_size);
			std::ifstream in(s);
			if (in.fail()) {
				set_translation();
				std::ofstream out(s);
				for (size_t i = 0; i < m_num_function; ++i) 
					for (size_t j = 0; j < m_variable_size; ++j) 
						out << m_function[i]->translation()[j] << " ";
				
				out.close();
			}
			else {
				for (size_t i = 0; i < m_num_function; ++i) 
					for (size_t j = 0; j < m_variable_size; ++j) 
						in >> m_function[i]->translation()[j];
			}
			in.close();
			for(auto &i: m_function)
				i->set_tranlation_flag(true);
			return true;
		}
		bool composition::load_rotation(const std::string &path) {
			std::string s;
			std::stringstream ss;
			ss << m_variable_size << "Dim.txt";
			s = ss.str();
			s.insert(0, m_name + "_RotM_");

			s.insert(0, path);// data path
			s.insert(0, global::ms_arg[param_workingDir]);

			for (auto &i : m_function)
				i->rotation().resize(m_variable_size, m_variable_size);
			std::ifstream in(s);
			if (in.fail()) {
				set_rotation();
				std::ofstream out(s);
				for (size_t i = 0; i < m_num_function; ++i)
					m_function[i]->rotation().print(out);
				out.close();
			}
			else {
				for (size_t i = 0; i < m_num_function; ++i) 
					m_function[i]->rotation().load(in);	
			}
			in.close();
			for (auto &i : m_function)
				i->set_rotation_flag(true);
			return true;
		}
		void composition::set_translation() {
			for (int i = 0; i < m_num_function; i++) 
				for (int j = 0; j < m_variable_size; j++) 
					m_function[i]->translation()[j] = m_domain[j].limit.first + (m_domain[j].limit.second - m_domain[j].limit.first)*(1 - global::ms_global->m_uniform[caller::Problem]->next());
		}
		void composition::set_rotation() {
			for (auto i : m_function) 
				i->rotation().generate_rotation_classical(global::ms_global->m_normal[caller::Problem].get(), i->condition_number());
				
		}
		function* composition::get_function(size_t num) {
			return m_function[num];
		}
		evaluation_tag composition::evaluate_(base &s, caller call, bool effective_fes, bool constructed) {
			variable<real> &x = dynamic_cast< solution<variable<real>, real> &>(s).get_variable();
			auto & obj = dynamic_cast< solution<variable<real>, real> &>(s).get_objective();
			

			std::vector<real> x_(x.begin(), x.end()); //for parallel running

			evaluate__(x_.data(), obj);

			if (constructed) {
				if (effective_fes)		m_effective_eval++;

				if (m_variable_monitor) {
					m_optima.is_optimal_variable(dynamic_cast<solution<variable<real>, real> &>(s), m_variable_accuracy);
					if (m_optima.is_variable_found())
						m_solved = true;
				}
				if (m_objective_monitor) {
					m_optima.is_optimal_objective(obj, m_objective_accuracy);
					if (m_optima.is_objective_found())
						m_solved = true;
				}
				if (call == caller::Algorithm&& global::ms_global->m_algorithm&&global::ms_global->m_algorithm->terminating())
					return evaluation_tag::Terminate;

				//if (mode == Program_Algorithm&&Global::msp_global->mp_problem && !Global::msp_global->mp_problem->isProTag(MOP)) m_globalOpt.isFound(s, m_disAccuracy, m_accuracy);
				//if (Global::msp_global != nullptr&&Global::msp_global->mp_algorithm != nullptr&&Global::msp_global->mp_algorithm->ifTerminating()) { return Return_Terminate; }
			}
			return evaluation_tag::Normal;
		}
	}
}

