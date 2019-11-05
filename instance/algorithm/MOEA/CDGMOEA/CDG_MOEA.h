/*************************************************************************
* Project: Library of Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Changhe Li & Xiaofang Wu
* Email: changhe.lw@google.com
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*************************************************************************
*  See the details of CDG-MOEA in the following paper
*  X.Cai, Z.Mei, Z.Fan, et al. A Constrained Decomposition Approach with Grids for Evolutionary 
*  Multiobjective Optimization. IEEE Transactions on Evolutionary Computation, 2017:1-1.
*************************************************************************/
// Created: 4 May 2019
// Last modified: 24 Aug 2019 by Xiaofang Wu (email:wuxiaofang@cug.edu.cn)


#ifndef OFEC_CDG_MOEA_H
#define OFEC_CDG_MOEA_H

#include"../../DE/individual.h"
#include"../../DE/population.h"
#include"../../DE/MOEA_DE/MOEA_DE.h"
#include <algorithm>


namespace OFEC {
	namespace CDG_MOEA {
		/******************************individual*************************/
		class individual :public OFEC::DE::individual {
			std::vector<int> m_Gk;
			std::vector<size_t> m_GN;
			std::vector<int> m_R;
			double m_meta = 20;
		public:
			using individual_type = OFEC::DE::individual;
			individual() :individual_type() {
				int M = global::ms_global->m_problem->objective_size();
				m_Gk.resize(M);
				m_R.resize(M);
				for (int i = 0; i < M; ++i)
					m_R[i] = 0;
			}
			template<typename ... Args>
			individual(size_t no, Args&& ... args) : individual_type(no, std::forward<Args>(args)...) {
				int M = global::ms_global->m_problem->objective_size();
				m_Gk.resize(M);
				m_R.resize(M);
				for (int i = 0; i < M; ++i)
					m_R[i] = 0;
			}
						
			std::vector<int>& Gk() { return m_Gk; }
			const std::vector<int>& Gk()const { return m_Gk; }
			std::vector<size_t>& GN() { return m_GN; }
			const std::vector<size_t>& GN() const { return m_GN; }
			std::vector<int>& R() { return m_R; }
			const std::vector<int>& R() const { return m_R; }
		};

		/******************************CDGMOEA_pop*************************/
		class CDGMOEA_pop : public OFEC::DE::MOEA_DE_pop<individual> {
		protected:
			std::vector<double> m_ideal;
			std::vector<double> m_nadir;
			std::vector<std::pair<real, real>> m_grid_min_max;	//first is m_grid_min,and second is m_grid_max
			std::vector<real> m_grid_distance;
			int m_grid_div;	//division numbers K=180 for bi-objective, K=30 for tri-objective
			int m_T;	//neighborhood distance set 5 for bi-objective and 1 for tri-objective
			double m_delta = 0.9;
			std::vector<std::vector<std::vector<int>>> m_S;
			std::vector<std::vector<int>> m_R;
			std::vector<individual> m_offspring;	//store P and Q			

		public:
			using population_type = OFEC::DE::MOEA_DE_pop<individual>;
			CDGMOEA_pop(size_t size_pop);
			void initialize();
			std::vector<double>& ideal() { return m_ideal; }
			const std::vector<double>& ideal() const { return m_ideal; }
			std::vector<double>& nadir() { return m_nadir; }
			const std::vector<double>& nadir() const { return m_nadir; }
			std::vector<std::vector<std::vector<int>>>& S() { return m_S; }
			const std::vector<std::vector<std::vector<int>>>& S() const  { return m_S; }
			std::vector<std::vector<int>>& R() { return m_R; }
			const std::vector<std::vector<int>>& R() const { return m_R; }
			void update_ideal_point();
			void update_ideal_point(const std::vector<individual> &offspring);
			void update_nadir_point();
			void update_nadir_point(const std::vector<individual> &offspring);
		
			void gridConstruct();	// construct grid environment
			void assign_GN();
			void gridConstruct_assignGN_P_reserve(std::vector<individual> &offspring, std::vector<int>& P_reserve, int size);
			void assign_S(std::vector<individual> &offspring, std::vector<int> P_reserve);
			void RBS(std::vector<individual> &offspring, std::vector<int> P_reserve);

			void select_random(int number, std::vector<int>& result);
			void nondominated_sorting(std::vector<individual> &offspring);
			
			//void record();
			//void record_x();
			//void record_x_offspring();
			//void record_f();
			//void record_f_offspring();
			evaluation_tag evolve();
			evaluation_tag evolve_mo();
			void eval_dens();

		private:
			void sortl(std::vector<std::pair<double, int>>& v);
			void sortR(const std::vector<int>& v1, std::vector<int>& v2);
			void sortL(std::vector<std::vector<int>>& R, std::vector<int>& number);			
		};		

		/******************************CDGMOEA****************************/
		class CDGMOEA : public algorithm {
		protected:			
			CDGMOEA_pop m_pop;
		public:
			CDGMOEA(param_map &v);			
			void initialize() override;
			void record() override;
		protected:
			void run_() override;
		};

	}

	using CDGMOEA = CDG_MOEA::CDGMOEA;
}


#endif
