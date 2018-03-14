/******************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*******************************************************************************
* Author: Changhe Li
* Email: changhe.lw@gmail.com
* Language: C++
*-------------------------------------------------------------------------------
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*
*  see https://github.com/Changhe160/OFEC for more information
*
*-------------------------------------------------------------------------------
* class optima stores a set of optimal solutions for a given problem
*
*********************************************************************************/
#ifndef OFEC_OPTIMA_H
#define OFEC_OPTIMA_H
#include "../algorithm/solution.h"

namespace OFEC {

	template<typename VariableEncoding = variable<real>,
		typename ObjetiveType = real
	>
	class optima {
		using objective_type = ObjetiveType;
		using variable_encoding = VariableEncoding;
	public:
		optima() = default;

		void append(const variable_encoding &x) {
			m_var.push_back(std::make_pair(x, false));
			++m_number_var;
		}
		void append(const std::vector<objective_type> &obj) {
			m_obj.push_back(std::make_pair(obj, false));
		}
		void append(const objective_type obj) {
			m_obj.push_back(std::make_pair(std::vector<objective_type>(1, obj), false));
		}
		size_t number_variable() const {
			return m_number_var;
		}

		bool variable_given() const {
			return !m_var.empty();
		}
		bool objective_given() const {
			return !m_obj.empty();
		}
		void set_number_variable(size_t n) {
			m_number_var = n;
		}
		void resize_variable(size_t n) { // reallocate memory for each variable
			for (auto &i : m_var) {
				i.first.resize(n);
			}
		}
		void resize_objective(size_t n) {// reallocate memory for each objecitve
			for (auto &i : m_obj) {
				i.first.resize(n);
			}
		}
		void resize_variable_set(size_t n) {
			m_var.resize(n);
		}
		void resize_objective_set(size_t n) {
			m_obj.resize(n);
		}
		const variable_encoding& variable(size_t i)const {
			return m_var[i].first;
		}
		variable_encoding& variable(size_t i) {
			return m_var[i].first;
		}
		void set_variable(const variable_encoding &x, size_t i) {
			m_var[i].first = x;
		}
		void set_objective(const std::vector<objective_type> &obj, size_t i) {
			m_obj[i].first = obj;
		}

		objective_type single_objective(size_t i = 0) const {
			return m_obj[i].first[0];
		}
		const std::vector<objective_type>& multi_objective(size_t i = 0) const {
			return m_obj[i].first;
		}

		double distance_to_optimal_obj(objective_type o)const {
			return fabs(m_obj[0].first - o);
		}

		/*IGD(Inverted Generational Distance)
		R. Venkata Rao and Vivek Patel,
		"A multi-objective improved teaching-learning based optimization algorithm for unconstrained and constrained optimization problems,"
		International Journal of Industrial Engineering Computations 5 (2014) 1¨C22
		*/
		//return IGD(Inverted Generational Distance) of pop to optima 
		template<typename Population>
		double distance_to_optimal_obj(const Population &pop)const {
			double distance = 0;
			for (auto &i : m_obj) {
				double min_d = std::numeric_limits<double>::max();
				for (int j = 0; j < pop.size(); ++j) {
					double d = euclidean_distance(pop[j].get_objective().begin(), pop[j].get_objective().end(), i.first.begin());
					if (d<min_d)  min_d = d;
				}
				distance += min_d;
			}
			return distance / m_obj.size();

		}

		template<typename Solution>
		bool is_optimal_variable(const Solution &s, optima &opt_found, double epsilon) {
			bool flag = false;
			for (size_t i = 0; i < m_var.size(); ++i) {
				if (m_var[i].second) continue;
				if (s.variable_distance(m_var[i].first) <= epsilon) {
					m_var[i].second = true;
					flag = true;
					opt_found.append(s.get_variable());   // record the variable found
					opt_found.append(s.get_objective());
					opt_found.m_var[i].second = true;
					opt_found.m_obj[i].second = true;
				}
			}
			return flag;
		}

		template<typename Solution>
		bool is_optimal_objective(const Solution &s, optima &opt_found, double epsilon_obj, double epsilon_var) {  //  for multi-modal opt
			for (size_t i = 0; i < m_obj.size(); ++i) {
				if (m_obj[i].second) continue;
				double dis_obj = euclidean_distance(s.get_objective().begin(), s.get_objective().end(), m_obj[i].first.begin());
				if (dis_obj <= epsilon_obj) {
					for (size_t j = 0; j < m_obj.size(); ++j) {
						if (m_obj[j].second && euclidean_distance(m_obj[j].first.begin(), m_obj[j].first.end(), m_obj[i].first.begin()) <= epsilon_obj &&
							s.variable_distance(opt_found.variable(j)) <= epsilon_var)
							return false;				
					}
					m_obj[i].second = true;
					
					opt_found.append(s.get_variable());   // record the variable found
					opt_found.append(s.get_objective());
					opt_found.m_var[i].second = true;
					opt_found.m_obj[i].second = true;
					return true;
				}
			}
			return false;
		}

		bool is_optimal_objective(const std::vector<objective_type> &o, double epsilon) {  // for multi-objective opt
			bool flag = false;
			for (auto &i : m_obj) {
				if (i.second) continue;
				double d = euclidean_distance(o.begin(), o.end(), i.first.begin());
				if (d <= epsilon) {
					i.second = true;
					flag = true;
				}
			}
			return flag;
		}

		bool is_optimal_objective(const objective_type &o, double epsilon) { // single objective optimization problem
			if (m_obj[0].second) return true;

			return m_obj[0].second = fabs(m_obj[0].first[0] - o) <= epsilon;
		}

		bool is_variable_found() {
			for (auto &i : m_var) {
				if (!i.second) return false;
			}
			return true;
		}
		bool is_objective_found() {
			for (auto &i : m_obj) {
				if (!i.second) return false;
			}
			return true;
		}
		void clear() {
			m_var.clear();
			m_obj.clear();
			m_number_var = 0;
		}
		int num_optima_found() const {
			if (dynamic_cast<continuous*>(global::ms_global->m_problem.get())->variable_monitor())
				return num_variable_found();
			else if(dynamic_cast<continuous*>(global::ms_global->m_problem.get())->objective_monitor())
				return num_objective_found();
			else throw myexcept("No monitor!");
		}
		template<typename Solution>
		void update_objective() {
			//resize_objective_set(m_number_var);
			for (size_t i = 0; i < m_number_var; ++i) {
				if (!(m_var[i].second)) continue;
				objective<ObjetiveType> temp_obj(GET_NUM_OBJ);
				Solution temp(m_var[i].first, temp_obj);
				global::ms_global->m_problem.get()->evaluate_(temp, caller::Problem, false, false);
				append(temp.get_objective());
				m_obj[i].second = true;
			}
		}
		const bool variable_flag(size_t i)const {
			return m_var[i].second;
		}
		const bool objective_flag(size_t i)const {
			return m_obj[i].second;
		}
	protected:
		int num_variable_found() const {
			int count = 0;
			for (auto &i : m_var)
				if (i.second) ++count;
			return count;
		}
		int num_objective_found() const {
			int count = 0;
			for (auto &i : m_obj)
				if (i.second) ++count;
			return count;
		}
	private:
		std::vector<std::pair<variable_encoding, bool>> m_var;
		std::vector<std::pair<std::vector<objective_type>, bool>> m_obj;
		size_t m_number_var = 0;
	};


}
#endif // !OFEC_OPTIMA_H