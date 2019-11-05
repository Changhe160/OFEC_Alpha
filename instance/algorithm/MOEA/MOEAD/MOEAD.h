/*************************************************************************
* Project: Library of Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Changhe Li & Yong Xia
* Email: changhe.lw@google.com
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.

*************************************************************************/
// Created: 13 Jan 2015
// Last modified: 25 Aug 2019 by Xiaofang Wu (email:wuxiaofang@cug.edu.cn)

#ifndef OFEC_MOEAD_H
#define OFEC_MOEAD_H

#include<algorithm>
#include<iomanip>
#include<fstream>
#include "../../DE/MOEA_DE/MOEA_DE.h"
#include "../../../../utility/matrix.h"
#include "../../../../utility/random/newran.h"

namespace OFEC {
	template<typename Individual>
	class MOEAD{
		enum DecomFun { _TCHE1, _TCHE2, _NBI1, _NBI2, _NBI3 };
	public:
		MOEAD();
		evaluation_tag evolve();
		virtual evaluation_tag evolve_mo() = 0;
	protected:
		void initialize(std::vector<std::unique_ptr<Individual>> &parent);
		void init_uniformweight(int parent_size);
		void init_neighbourhood();
		void update_reference(Individual &sol);
		void update_problem(std::vector<std::unique_ptr<Individual>> &parent, Individual &sol, int id, int type);
		real fitnessfunction(std::vector<real> &obj, int k);
		void matingselection(std::vector<int> &list, int cid, int size, int type, int parent_size);
		std::vector<real> mv_ideal_point;	//the best value in every dimension
		std::vector<Individual> mv_sol_arr;	//corresponding Individual of the best value in every dimension
		std::vector<Vector> mv_namda;
		std::vector<std::vector<int> > mvv_neigh;
		int m_unit;
		int m_limit;
		int m_niche;		//number of neighbours
		real m_realb;     // probability of selecting mating parents from neighborhood
		DecomFun m_decom_function;
	private:
		void min_max_sort(std::vector<real> &v, std::vector<int> &idx);
	};

	template<typename Individual>
	MOEAD<Individual>::MOEAD():m_unit(13), m_niche(20), m_realb(0.9), m_limit(2), m_decom_function(_TCHE1) {
		int numObj = global::ms_global->m_problem->objective_size();
		mv_sol_arr.resize(numObj);
		mv_ideal_point.resize(numObj, 1.0e+30);	
	}

	template<typename Individual>
	evaluation_tag MOEAD<Individual>::evolve() {
		//std::cout << "the " << this->m_iter << " generation" << std::endl;
		evaluation_tag tag = evaluation_tag::Normal;
		tag = evolve_mo();		
		return tag;
	}

	template<typename Individual>
	void MOEAD<Individual>::initialize(std::vector<std::unique_ptr<Individual>> &parent) {
		init_uniformweight(parent.size());
		init_neighbourhood();
		for (int i = 0; i < parent.size(); i++)
			update_reference(*(parent[i]));
	}

	template<typename Individual>
	void MOEAD<Individual>::init_uniformweight(int parent_size) {		
		if (global::ms_global->m_problem->objective_size() == 2){
			mv_namda.resize(parent_size);
			for (int n = 0; n < parent_size; n++){
				real a = 1.0*n / (parent_size - 1);
				mv_namda[n].push_back(a);
				mv_namda[n].push_back(1 - a);
			}
		}
		else{
			int n = 0;
			for (int i = 0; i <= m_unit; i++){
				for (int j = 0; j <= m_unit; j++){
					if (i + j <= m_unit){
						std::vector<int> arr;
						arr.push_back(i);
						arr.push_back(j);
						arr.push_back(m_unit - i - j);
						mv_namda.push_back(std::vector<real>(0));
						for (int k = 0; k < arr.size(); k++)
							mv_namda[n].push_back(1.0*arr[k] / m_unit);
						n++;
						arr.clear();
					}
				}
			}
		}
	}
	
	template<typename Individual>
	void MOEAD<Individual>::init_neighbourhood() {
		int pops = mv_namda.size();
		mvv_neigh.resize(pops);
		std::vector<real> dis(pops);
		std::vector<int> index(pops);
		for (int i = 0; i < pops; i++){
			// calculate the distances based on weight vectors
			for (int j = 0; j < pops; j++){
				dis[j] = mv_namda[i].distance(mv_namda[j]);
				index[j] = j;
			}
			//find 'niche' nearest neighboring subproblems			
			min_max_sort(dis, index);
			for (int k = 0; k < m_niche; k++)
				mvv_neigh[i].push_back(index[k]);
		}
		dis.clear();
		index.clear();
	}

	template<typename Individual>
	void MOEAD<Individual>::update_reference(Individual &sol) {
		//sol: child Individual
		int numObj = global::ms_global->m_problem->objective_size();		
		for (int n = 0; n < numObj; n++){
			if (sol.objective()[n] < mv_ideal_point[n]){
				mv_ideal_point[n] = sol.objective()[n];
				mv_sol_arr[n] = sol;
			}
		}		
	}

	template<typename Individual>
	void MOEAD<Individual>::update_problem(std::vector<std::unique_ptr<Individual>> &parent, Individual &sol, int id, int type) {
		// sol: child Individual
		// id:  the id of current subproblem
		// type:update Individuals in - neighborhood (1) or whole population (otherwise)
		int size, time = 0;
		if (type == 1)	
			size = m_niche;
		else        
			size = parent.size();
		std::vector<int> perm(size);
		for (int i(0); i < perm.size(); ++i) {
			perm[i] = i;
		}
		global::ms_global->m_uniform[caller::Algorithm]->shuffle(perm.begin(), perm.end());

		for (int i = 0; i < size; i++){
			int k;
			if (type == 1) 
				k = mvv_neigh[id][perm[i]];
			else        
				k = perm[i];
			// calculate the values of objective function regarding the current subproblem
			real f1, f2;
			f1 = fitnessfunction(parent[k]->objective(), k);
			f2 = fitnessfunction(sol.objective(), k);
			if (f2 < f1){
				*parent[k] = sol;
				time++;
			}
			// the maximal number of Individual updated is not allowed to exceed 'limit'
			if (time >= m_limit)
				return;
		}
		perm.clear();
	}

	template<typename Individual>
	real MOEAD<Individual>::fitnessfunction(std::vector<real> &obj, int k) {
		// Chebycheff Scalarizing Function
		real fitness = 0;
		int numObj = global::ms_global->m_problem->objective_size();
		//int numObj = obj.size();
		if (m_decom_function == _TCHE1){
			real max_fun = -1.0e+30;
			for (int n = 0; n < numObj; n++){
				//real diff = fabs(y_obj[n] - idealpoint[n] + scale[n]);
				//real diff = fabs(y_obj[n] - idealpoint[n] + 0.05);
				real diff = fabs(obj[n] - mv_ideal_point[n]);
				//real diff = fabs(y_obj[n] - 0);
				real feval;
				if (mv_namda[k][n] == 0)
					feval = 0.0001*diff;
				else
					feval = diff * mv_namda[k][n];
				if (feval > max_fun) max_fun = feval;

			}
			fitness = max_fun;
		}

		if (m_decom_function == _TCHE2)	{
			// reference point in the CHIM
			std::vector<int> scale(numObj);
			//throw myException("Please initialize the scale @MOEAD<Poppulation,Individual>::fitnessfuction");
			real max_fun = -1.0e+30;
			for (int n = 0; n < numObj; n++){
				real diff = (obj[n] - mv_ideal_point[n]) / scale[n];  //note: the scale is not initialized, there has no knowledge
				real feval;
				if (mv_namda[k][n] == 0)
					feval = 0.0001*diff;
				else
					feval = diff * mv_namda[k][n];
				if (feval > max_fun) max_fun = feval;

			}
			fitness = max_fun;
		}

		// CHIM + Tchebycheff
		// CHIM is not available in 3 objectives
		if (m_decom_function == _NBI1) {
			// quasi normal direction
			Vector norm;
			for (int i = 0; i < numObj; i++){
				norm.push_back(0.0);
				for (int j = 0; j < numObj; j++) {
					norm[i] += -mv_sol_arr[j].objective()[i];
				}
			}

			// normalization
			norm.normalize();

			// reference point in the CHIM
			std::vector <real> base;
			for (int i = 0; i < numObj; i++){
				real tp2 = 0;
				for (int j = 0; j < numObj; j++)
					tp2 += mv_sol_arr[j].objective()[i] * mv_namda[k][j];
				base.push_back(tp2);
			}

			// Tchebycheff function
			real max_fun = -1.0e+30;
			for (int n = 0; n < numObj; n++){
				real diff = obj[n] - base[n];
				real feval = -diff * norm[n];
				if (feval > max_fun) max_fun = feval;

			}
			fitness = max_fun;
		}

		//* Boundary intersection approach
		//* reference point is chosen as the ideal point
		//* the direction is independent of CHIM
		if (m_decom_function == _NBI2){

			mv_namda[k].normalize();

			// penalty method 
			// temporary vectors NBI method
			Vector realA(numObj);
			Vector realB(numObj);

			// difference beween current point and reference point
			for (int n = 0; n < numObj; n++)
				realA[n] = (obj[n] - mv_ideal_point[n]);

			// distance along the search direction norm
			real d1 = fabs(realA*mv_namda[k]);

			// distance to the search direction norm
			for (int n = 0; n < numObj; n++)
				realB[n] = (obj[n] - (mv_ideal_point[n] + d1 * mv_namda[k][n]));
			real d2 = realB.length();

			fitness = (d1 + 5 * d2);

			//t2 = clock();
			//total_sec+=(t2 - t1);
		}

		// NBI method
		if (m_decom_function == _NBI3) {

			// quasi normal direction
			Vector norm;
			for (int i = 0; i < numObj; i++){
				norm.push_back(0.0);
				for (int j = 0; j < numObj; j++) {
					norm[i] += -mv_sol_arr[j].objective()[i];
				}
			}

			// normalization
			norm.normalize();

			// reference point in the CHIM
			std::vector<real> base;
			for (int i = 0; i < numObj; i++){
				real tp2 = 0;
				for (int j = 0; j < numObj; j++)
					tp2 += mv_sol_arr[j].objective()[i] * mv_namda[k][j];
				base.push_back(tp2);
			}

			// penalty method 
			// temporary vectors NBI method
			Vector realA;
			Vector realB;

			// difference beween current point and reference point
			for (int n = 0; n < numObj; n++)
				realA.push_back(obj[n] - base[n]);

			// distance along the search direction norm
			real d1 = realA * norm;

			// distance to the search direction norm
			for (int n = 0; n < numObj; n++)
				realB.push_back(obj[n] - (base[n] + d1 * norm[n]));
			real d2 = realB.length();

			fitness = -d1 + 2 * d2;
		}
		return fitness;
	}

	template<typename Individual>
	void MOEAD<Individual>::matingselection(std::vector<int> &list, int cid, int size, int type, int parent_size) {
		// list : the set of the indexes of selected mating parents
		// cid  : the id of current subproblem
		// size : the number of selected mating parents
		// type : 1 - neighborhood; otherwise - whole population
		int r, p;
		while (list.size() < size){
			if (type == 1) {
				r = global::ms_global->m_uniform[caller::Algorithm]->next_non_standard(0, m_niche);				
				p = mvv_neigh[cid][r];
			}
			else
				p = global::ms_global->m_uniform[caller::Algorithm]->next_non_standard(0, parent_size);
				
			bool flag = true;
			for (int i = 0; i < list.size(); i++){
				if (list[i] == p){ // p is in the list				
					flag = false;
					break;
				}
			}

			if (flag) list.push_back(p);
		}
	}


	template<typename Individual>
	void MOEAD<Individual>::min_max_sort(std::vector<real> &v, std::vector<int> &idx) {
		for (int i = 0; i < v.size(); ++i) {
			int min = i;
			for (int j = i + 1; j < v.size(); ++j) {
				if (v[j] < v[min])
					min = j;
			}
			auto temp = v[i];
			v[i] = v[min];
			v[min] = temp;
			auto t = idx[i];
			idx[i] = idx[min];
			idx[min] = t;
		}
	}
}

#endif