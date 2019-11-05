/*************************************************************************
* Project: Library of Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Changhe Li & Yong Xia & Junchen Wang
* Email: changhe.lw@google.com
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*************************************************************************/
// Created: 7 Jan 2015
// Last modified: 15 Aug 2019 by Junchen Wang (email:wangjunchen.chris@gmail.com)

/*-----------------------------------------------------------------------------------
   See the details of NSGA2 in the following paper
   A Fast and Elitist Multiobjective Genetic Algorithm : NSGA - II
   Kalyanmoy Deb, Associate Member, IEEE, Amrit Pratap, Sameer Agarwal, and T.Meyarivan
   IEEE TRANSACTIONS ON EVOLUTIONARY COMPUTATION, VOL. 6, NO. 2, APRIL 2002
-----------------------------------------------------------------------------------*/

#ifndef OFEC_NSGAII_H
#define OFEC_NSGAII_H

#include "../../../../utility/nondominated_sorting/fast_sort.h"

namespace OFEC {

	template<typename Individual>
	class NSGAII {
	public:
		using objective_type = typename Individual::solution_type::objective_encoding;
	public:
		NSGAII() {}
	protected:
		void survivor_selection(std::vector<std::unique_ptr<Individual>> &parent, std::vector<Individual> &offspring);
		void nondominated_sorting(std::vector<Individual>& offspring);
	private:		
		void eval_dens(std::vector<std::unique_ptr<Individual>>& parent, std::vector<Individual>& offspring);
	};

	template<typename Individual>
	void NSGAII<Individual>::survivor_selection(std::vector<std::unique_ptr<Individual>>& parent, std::vector<Individual>& offspring) {
		nondominated_sorting(offspring);
		eval_dens(parent, offspring);
	}

	template<typename Individual>
	void NSGAII<Individual>::nondominated_sorting(std::vector<Individual>& offspring) {
		std::vector<std::vector<objective_type>*> objs;
		for (auto& i : offspring)
			objs.emplace_back(&i.objective());
		std::vector<int> rank;
		NS::fast_sort<objective_type>(objs, rank, global::ms_global->m_problem->opt_mode());
		for (size_t i = 0; i < offspring.size(); ++i)
			offspring[i].set_rank(rank[i]);
	}

	template<typename Individual>
	void NSGAII<Individual>::eval_dens(std::vector<std::unique_ptr<Individual>>& parent, std::vector<Individual>& offspring) {
		std::vector<Individual*> pop;
		for (auto& i : offspring)
			pop.emplace_back(&i);

		int numobj = global::ms_global->m_problem->objective_size();
		int pops = 0;  //indicate parent population size be 0
		int size = pop.size();
		int rank = 0;
		while (true) {
			int count = 0;
			for (size_t i = 0; i < size; i++)
				if (pop[i]->rank() == rank)
					count++;
			int size2 = pops + count;
			if (size2 > parent.size()) {
				break;
			}
			for (size_t i = 0; i < size; i++)
				if (pop[i]->rank() == rank)
				{
					*parent[pops] = offspring[i];
					++pops;
				}
			rank++;
			if (pops >= parent.size()) break;
		}
		if (pops < parent.size()) {
			std::vector<int> list;
			// save the individuals in the overflowed front
			for (size_t i = 0; i < size; i++)
				if (pop[i]->rank() == rank)
					list.push_back(i);
			int s2 = list.size();
			std::vector<real> density(s2);
			std::vector<real> obj(s2);
			std::vector<int> idx(s2);
			std::vector<int> idd(s2);
			for (size_t i = 0; i < s2; i++) {
				idx[i] = i;
				density[i] = 0;
			}
			for (size_t j = 0; j < numobj; j++) {
				for (size_t i = 0; i < s2; i++) {
					idd[i] = i;
					obj[i] = pop[list[i]]->objective()[j];
				}
				merge_sort(obj, s2, idd, true, 0, s2 - 1, s2);
				density[idd[0]] += -1.0e+30;
				density[idd[s2 - 1]] += -1.0e+30;
				for (int k = 1; k < s2 - 1; k++)
					density[idd[k]] += -(obj[idd[k]] - obj[idd[k - 1]] + obj[idd[k + 1]] - obj[idd[k]]);
			}
			idd.clear();
			obj.clear();
			int s3 = parent.size() - pops;
			merge_sort(density, s2, idx, true, 0, s2 - 1, s3);
			for (size_t i = 0; i < s3; i++) {
				*parent[pops] = offspring[list[idx[i]]];
				++pops;
			}
			density.clear();
			idx.clear();
			list.clear();
		}
	}
}
#endif //!OFEC_NSGAII_H
