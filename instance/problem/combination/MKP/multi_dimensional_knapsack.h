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
//ע�� �����ⱻת��Ϊ��С������, �������ý��û�о�ȷ����Ƿ��ǺϷ���
// Modified: 14 Mar 2018 By Junchen Wang (wangjunchen@cug.edu.cn)

#ifndef MKP_H
#define MKP_H

#include "../../../../core/problem/problem.h"
#include "../../../../core/problem/optima.h"
#include "../../../../core/problem/domain.h"

namespace OFEC {
#define CAST_MKP dynamic_cast<multi_dimensional_knapsack *>(global::ms_global->m_problem.get());

	class multi_dimensional_knapsack : public problem {
	protected:
		std::vector<double> mv_p;
		std::vector<std::vector<double>> mvv_r;
		std::vector<double> mv_b;
		std::string m_file_name;
		optima<variable<int>, real> m_optima;
		int m_m;
		double m_maxP;
		bool m_if_valid_check = true;
	public:
		multi_dimensional_knapsack(param_map& v);
		multi_dimensional_knapsack(const std::string & pro_name, size_t size_var, const std::string & file_name);
		~multi_dimensional_knapsack() {};
		evaluation_tag evaluate_(base &s, caller call, bool effective_fes, bool constructed);
		bool is_valid(const base &s) const;
		void read_problem();
		int invalid_constrain_num(base &s_) const;
		bool get_optimal_obj(std::vector<double> &opt) const;
		bool get_optimal_obj(std::vector<std::vector<double>> &opt) const;
		const optima<variable<int>, real>& get_optima() const;
		optima<variable<int>, real>& get_optima();
		bool is_optima_given() const;
		multi_dimensional_knapsack* get_type_ptr();
		multi_dimensional_knapsack& get_type_ref();
		bool if_optima_found();
		double get_constraint_value(const base& s, std::vector<double>& val) const;
		void initialize_solution(base &s) const;
		bool same(const base &s1, const base &s2) const;
		double variable_distance(const base &s1, const base &s2) const;
		double variable_distance(const variable_base &s1, const variable_base &s2) const;
		void initialize();
	};
}

#endif // !MKP_H
