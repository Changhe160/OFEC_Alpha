/*************************************************************************
* Project: Library of Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Changhe Li & Yong Xia
* Email: changhe.lw@google.com  Or cugxiayong@gmail.com
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*************************************************************************/
// Created: 18 Apr 2016
// Modified: 14 Mar 2018 By Junchen Wang (wangjunchen@cug.edu.cn)

#ifndef ONE_MAX_H
#define ONE_MAX_H

#include "../../../../core/problem/problem.h"
#include "../../../../core/problem/optima.h"

namespace OFEC {
#define CAST_one_max dynamic_cast<one_max*>(global::ms_global->m_problem.get());

	class one_max : public problem {
	protected:
		optima<variable_vector<int>, real> m_optima;
		bool m_if_valid_check = true;
	public:
		one_max(param_map& v);
		one_max(const std::string &name, int size_var);
		~one_max() {};
		evaluation_tag evaluate_(solution_base &s, caller call, bool effective, bool initialized);
		bool is_valid(const solution_base &s);
		void validate(solution_base &s) {};
		void initialize_solution(solution_base &s) const override;
		bool same(const solution_base &s1, const solution_base &s2) const override;
		real variable_distance(const solution_base &s1, const solution_base &s2) const override;
		real variable_distance(const variable_base &s1, const variable_base &s2) const override;
		bool is_optima_given() override;
		bool get_opt_obj(std::vector<real> &opt) {
			opt = m_optima.objective();
			return true;
		};
		bool get_opt_obj(std::vector<std::vector<real>> &value) {
			value.clear();
			for (unsigned i = 0; i<m_optima.number_objective(); i++)	
				value.push_back(m_optima.objective(i));
			return true;

		}
		const optima<variable_vector<int>, real>& get_optima()const {
			return m_optima;
		}
		optima<variable_vector<int>, real>& get_optima() {
			return m_optima;
		}
		void initialize();
	};

}

#endif // !ONE_MAX_H

