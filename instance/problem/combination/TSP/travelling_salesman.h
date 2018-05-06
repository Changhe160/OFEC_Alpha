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
// Created: 7 Oct 2014
// Modified: 14 Mar 2018 By Junchen Wang (wangjunchen@cug.edu.cn)

#ifndef TSP_H
#define TSP_H

#include "../../../../core/problem/problem.h"
#include "../../../../core/problem/optima.h"
#include "../../../../core/problem/domain.h"

namespace OFEC {
#define CAST_TSP dynamic_cast<travelling_salesman*>(global::ms_global->m_problem.get())
	class travelling_salesman : public problem {
	protected:
		std::vector<std::vector<std::vector<double>>> mvvv_cost;  // the cost between cities
		std::string m_file_name; // input tsp file name
		domain<int> m_domain; // boundary of each variable
		optima<variable<int>, real> m_optima; // the optimal solution of problem
		std::vector<std::vector<int>> mvv_nearby; // nearby cities of each city
		bool m_if_valid_check = true;

	public:
		travelling_salesman(param_map &v);
		travelling_salesman(const std::string& pro_name, int size_var, int size_obj, const std::string& file_name);
		evaluation_tag evaluate_(solution_base &s, caller call, bool effective_fes, bool constructed);
		bool is_valid(solution_base &s) const;
		void initialize_solution(solution_base &s) const;
		bool same(const solution_base &s1, const solution_base &s2) const;
		double variable_distance(const solution_base &s1, const solution_base &s2) const;
		double variable_distance(const variable_base &s1, const variable_base &s2) const;
		std::pair<int, int> get_next_city(const solution_base &s, int city) const; //return the next and previous cities of city in solution s

		void find_nearby_city(std::vector<std::vector<int>> &nearby,int num_near=0, int obj=0);     //find some percent of nearby city
		void prim(std::vector<std::vector<int>>& mst_edge, int n = 0); // find MST edges
		void calculate_edge_weight(char* edge_type, std::vector<std::vector<double>>& coordinate);

		const std::vector<std::vector<double>>& get_cost(int i = 0) const { return mvvv_cost[i]; }
		const std::vector<std::vector<int>>& get_near() const { return mvv_nearby; }
		std::string get_file_name() const { return m_file_name; }
		const optima<variable<int>, real>& get_optima() const { return m_optima; }
		optima<variable<int>, real>& get_optima() { return m_optima; };
		bool is_optima_given();
		bool is_optima_found();
		travelling_salesman* get_type_prt() { return this; }
		travelling_salesman& get_type_ptr() { return *this; }
		//virtual void create_candidate_sets(std::vector<std::vector<int>> &candidate_sets);  // LKH: create candidate sets
		void initialize();
	protected:
		virtual void read_problem();    //read source data from file
	};

	int select_city_random(std::vector<std::vector<int>>& matrix, std::vector<int>& visited, int num, int row);
	int select_city_random(std::vector<int> visited, int dim);
	int select_city_greedy(std::vector<std::vector<int>>& matrix, std::vector<int>& visited, int num, int row);
}

#endif // !TSP_H

