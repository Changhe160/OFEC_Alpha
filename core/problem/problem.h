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
* macros used in OFEC
*
*
*********************************************************************************/
#ifndef OFEC_PROBLEM_H
#define OFEC_PROBLEM_H
#include <string>
#include <limits>
#include <map>
#include <memory>
#include <set>
#include <sstream>

#include "../definition.h"
#include "../algorithm/encoding.h"

namespace OFEC {
	class problem {
	public:
		problem(const std::string &name, size_t size_var, size_t size_obj) :m_name(name), m_variable_size(size_var), 
			m_objective_size(size_obj), m_opt_mode(size_obj){

		}
		virtual ~problem() {}
		problem(const problem&) = delete;

		optimization_mode opt_mode(size_t idx) const {
			return m_opt_mode[idx];
		}
		const vector<optimization_mode>& opt_mode() const {
			return m_opt_mode;
		}
		virtual bool same(const base &s1, const base &s2) const = 0;
		virtual double variable_distance(const base &s1, const base &s2) const =0;
		virtual double variable_distance(const variable_base &s1, const variable_base &s2) const = 0;

		template<typename Solution>
		evaluation_tag evaluate(Solution &s, caller call, bool effective_eval = true) {

			evaluation_tag tag = evaluate_(s, call, effective_eval, true);

			++m_total_eval;

			return tag;
		}
		virtual evaluation_tag evaluate_(base &s, caller call, bool effective_fes, bool constructed)=0;

		const std::string& name() const{
			return m_name;
		}

		virtual violation_type check_boundary_violation(const base &) const { 
			return violation_type::None; 
		}
		virtual violation_type check_constraint_violation(const base &) const { 
			return violation_type::None;
		}
		virtual void constraint_value(const base &, std::pair<double,vector<double>>&)=0;
		
		template<typename Solution>
		static void initialize_objective_minmax(const Solution &s) {
			for (int i = 0; i < m_objective_size; ++i) {
				*ms_minmax_objective[i].first = s;
				*ms_minmax_objective[i].second = s;
			}
		}

		template<typename Solution>
		static void update_objective_minmax(const Solution &s) {
			for (int i = 0; i < m_objective_size; ++i) {
				if (m_opt_mode[i] == optimization_mode::Minimization) {
					if (ms_minmax_objective[i].first->get_objective()[i] > s.get_objective()[i])
						*ms_minmax_objective[i].first = s;
					if (ms_minmax_objective[i].second->get_objective()[i] < s.get_objective()[i])
						*ms_minmax_objective[i].second = s;
				}
				else {
					if (ms_minmax_objective[i].first->get_objective()[i] < s.get_objective()[i])
						*ms_minmax_objective[i].first = s;
					if (ms_minmax_objective[i].second->get_objective()[i] > s.get_objective()[i])
						*ms_minmax_objective[i].second = s;
				}
			}
		}

		template<typename Solution, typename... Args>
		static void allocate_memory(size_t no, Args&& ...args) {
			for (int i = 0; i < no; ++i) {
				ms_minmax_objective.emplace(i,
					make_pair(make_unique<Solution>(no, std::forward<Solution::variable_encoding>(args)...), make_unique<Solution>(no, std::forward<Solution::variable_encoding>(args)...)));
			}
		}

		virtual void initialize_solution(base &s) const = 0;
		
		virtual double feasible_ratio() { return 1.0; }

		size_t variable_size() const { 
			return m_variable_size; 
		}
		size_t objective_size() const {
			return m_objective_size; 
		}
		void set_tag(const set<problem_tag> &tag) {
			m_tag = tag;
		}

		void add_tag(problem_tag tag) {
			if (!is_tag(tag))	m_tag.insert(tag);
		}
		bool is_tag(problem_tag tag) {
			if (m_tag.find(tag) != m_tag.end())	return true;
			else return false;
		}
		bool solved() {
			return m_solved;
		}
		size_t evaluations() {
			return m_effective_eval;
		}
		size_t total_evaluations() {
			return m_total_eval;
		}
	protected:
		problem& operator=(const problem& rhs);  // assignment is not allowed outside
		problem& operator=(problem&& rhs);
		virtual void copy(const problem *); // copy parameter values of a problem when it changes
		virtual void resize_variable(size_t n);
		virtual void resize_objective(size_t n);
	protected:
		std::string m_name;
		size_t m_effective_eval = 0, m_total_eval = 0;
		size_t m_objective_size, m_variable_size;
		std::vector<optimization_mode> m_opt_mode;
		double m_objective_accuracy=1.0e-6;
		std::set<problem_tag> m_tag;
		std::stringstream m_paramters;
		bool m_solved = false;
#ifdef OFEC_CONSOLE
		static thread_local std::map<int,pair<unique_ptr<base>, unique_ptr<base>>> ms_minmax_objective; // the best and worst so far solutions of each objective 
#endif
#ifdef OFEC_DEMON
		static std::map<int, pair<unique_ptr<base>, unique_ptr<base>>> ms_minmax_objective; // the best and worst so far solutions of each objective 
#endif	
	
	};

}


#endif // !OFEC_PROBLEM_H
