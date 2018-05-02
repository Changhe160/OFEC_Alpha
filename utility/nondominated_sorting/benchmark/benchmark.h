/*************************************************************************
* Project: Library of Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Changhe Li & Junchen Wang
* Email: changhe.lw@gmail.com, wangjunchen@cug.edu.cn
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.

*************************************************************************/
// Created: 29 Mar 2018 by Junchen Wang
// Last modified:

#ifndef NODES_INITIALIZE_H
#define NODES_INITIALIZE_H

/*
This header file is used to generate test data for non-dominance sorting.
It has several initial objective-space distributions of solutions.
*/

#include "../../random/newran.h"
#include <math.h>
#include <fstream>
#include <sstream>
#include <vector>

namespace NS {

	// abstract base class of data generator (can not controlled the number of fronts)
	class data_generator {
	protected:
		const int m_num_obj;
		const int m_num_sol;
		const std::string m_name;
		std::vector<std::vector<double>> m_data;
		OFEC::uniform m_rand;
		void output_data(const std::stringstream& path);
		void shuffle_data();
	public:
		data_generator(const std::string& name, int num_sol, int num_obj, double rand_seed);
		virtual void update_data() = 0;
		const std::vector<std::vector<double>>& get_data() { return m_data; }
		virtual void output_data_set(int num_data);
		virtual void read_data(std::vector<std::vector<double>>& data, int number);
	};

	// uniformly distributed data generator 
	class benchmark1 final : public data_generator {
	public:
		benchmark1(int num_sol, int num_obj, double rand_seed);
		void update_data();
	};

	// abstract base class for generating data with user-defined number of fronts
	class fixed_num_front : public data_generator {
	protected:
		const int m_num_fro;
	public:
		fixed_num_front(const std::string& name, int num_sol, int num_obj, int num_fro, double rand_seed);
		void output_data_set(int num_data);
		void read_data(std::vector<std::vector<double>>& data, int number);
	};

	// data with user-defined number of fronts with concave fronts (generate data uniformly and slowly)
	class benchmark2 final : public fixed_num_front {
	public:
		benchmark2(int num_sol, int num_obj, int num_fro, double rand_seed);
		void update_data();
	};

	// data with user-defined number of fronts with convex fronts (generate data uniformly and slowly)
	class benchmark3 final : public fixed_num_front {
	public:
		benchmark3(int num_sol, int num_obj, int num_fro, double rand_seed);
		void update_data();
	};

	// data with user-defined number of fronts with concave fronts (generate data nonuniformly but quickly)
	class benchmark4 final : public fixed_num_front {
	public:
		benchmark4(int num_sol, int num_obj, int num_fro, double rank_seed);
		void update_data();
	};

	// data with user-defined number of fronts with convex fronts (generate data nonuniformly but quickly)
	class benchmark5 final : public fixed_num_front {
	public:
		benchmark5(int num_sol, int num_obj, int num_fro, double rank_seed);
		void update_data();
	};
}

#endif // !NODES_INITIALIZE_H

