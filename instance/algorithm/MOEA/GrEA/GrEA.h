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

*  See the details of CDG-MOEA in the following paper
*  S.Yang, M.Li, X.Liu, et al. A Grid-Based Evolutionary Algorithm for Many-Objective 
*  Optimization. IEEE Transactions on Evolutionary Computation, 2013, 17(5):721-736.
*************************************************************************/
// Created: 4 May 2019
// Last modified: 23 Aug 2019 by Xiaofang Wu(email:email:wuxiaofang@cug.edu.cn)

#ifndef OFEC_GrEA_H
#define OFEC_GrEA_H

#include<iostream>
#include<vector>
#include<memory>
#include "../../../../core/algorithm/individual.h"
#include "../../../../core/algorithm/population.h"
#include "../../../../core/problem/continuous/continuous.h"
#include "../../GA/SBX/SBX.h"
#include "../NSGAII/NSGAII.h"

namespace OFEC {
	namespace GREA {
		/***************************individual******************************/
		class individual :public OFEC::individual<> {
			std::vector<int> m_Gk;
			double m_GR;
			double m_GCD = 0;
			double m_GCPD;			
		public:
			using individual_type = OFEC::individual<>;
			individual() :individual_type() {
				m_Gk.resize(global::ms_global->m_problem->objective_size());
			}
			template<typename ... Args>
			individual(size_t no, Args&& ... args) :individual_type(no, std::forward<Args>(args)...) { 
				m_Gk.resize(global::ms_global->m_problem->objective_size()); 
			}
			auto Gk(int idx) { return m_Gk.begin() + idx; }
			const double& GR() const { return m_GR; }
			double& GR() { return m_GR; }
			const double& GCD() const { return m_GCD; }
			double& GCD() { return m_GCD; }
			const double& GCPD() const { return m_GCPD; }
			double& GCPD() { return m_GCPD; }			
		};

		/******************************population*************************/		
		class GrEA_pop :public SBX_pop<individual>, NSGAII<individual> {
		public:
			GrEA_pop(size_t size_pop);
			~GrEA_pop() {}
			void initialize() override;	
			evaluation_tag evolve() override;
			evaluation_tag evolve_mo();
			void eval_dens();			
			void gridConstruct();	// construct grid environment
			void gridConstruct_Fi(std::vector<individual> &offspring,std::vector<int>& Fi, int size);
			void assign_GR_GCPD();	// assign GR and GCPD for individuals in the population
			void assign_GR_GCPD_Fi(std::vector<individual> &offspring,std::vector<int>& Fi, int size);
			void assign_GCD();		// assign GCD for individuals in the population
			int check_dominance_grid(individual &a, individual &b);			
		
		protected:
			std::vector<std::pair<real, real>> m_ind_min_max; //first is m_ind_min,and second is m_ind_max
			std::vector<std::pair<real, real>> m_grid_min_max;
			std::vector<real> m_grid_distance;
			int m_grid_div = 8;
			std::vector<individual> m_offspring;  // 2 size of population			
		};

		/******************************GrEA*******************************/
		class GrEA :public algorithm {
		public:
			GrEA(param_map& v);
			void initialize() override;
			void record() override;
		protected:
			void run_() override;
		protected:
			GrEA_pop m_pop;
		};

	}
	
	using GrEA = GREA::GrEA;
}
#endif