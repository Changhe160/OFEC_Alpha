
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
* some general methods used in ofec are defined in this file, including distance
* metrics, solution domination relationship,
*
*********************************************************************************/

#ifndef OFEC_GENERAL_FINCTIONAL_H
#define OFEC_GENERAL_FINCTIONAL_H

#include<string>
#include<vector>
#include<functional>
#include<array>
#include<thread>
#include<sstream>
#include <map>
#include "../../core/random/newran.h"
#include "../../core/problem/solution.h"
#include "../../core/problem/problem.h"


namespace ofec {

	template<typename TIndivididual>
	void sortByBetterFitnessDisDominate(
		const std::vector<TIndivididual>& inds,
		std::vector<int>& fitness_order, std::vector<int>& belong_idx, std::vector<int>& distance_order, ofec::Problem* pro) {
		int size(inds.size());
		belong_idx.resize(size);
		fitness_order.resize(size);
		distance_order.resize(size);
		for (int idx(0); idx < size; ++idx) {
			fitness_order[idx] = idx;
			belong_idx[idx] = idx;
			distance_order[idx] = idx;
		}
		std::sort(fitness_order.begin(), fitness_order.end(), [&](
			int a, int b
			) {
			if (inds[a]->fitness() == inds[b]->fitness()) return a < b;
			else return inds[a]->fitness() > inds[b]->fitness();
		});


		std::vector<double> min_dis(size, std::numeric_limits<double>::max());
		for (int idx(0); idx < size; ++idx) {
			int cur_idx(fitness_order[idx]);
			auto& sol1(inds[cur_idx]);
			auto& cur_dis(min_dis[cur_idx]);
			auto& belong_idx(belong_idx[cur_idx]);
			double dis(0);
			double max_fitness(-std::numeric_limits<double>::max());
			for (int idy(0); idy < idx; ++idy) {
				auto& sol2(inds[fitness_order[idy]]);

				if (sol1->fitness() < sol2->fitness()) {
					dis = sol1->variableDistance(*sol2, pro);
					if (cur_dis > dis) {
						cur_dis = dis;
						belong_idx = fitness_order[idy];
					}
				}

			}
		}
		std::sort(distance_order.begin(), distance_order.end(),
			[&](int a, int b) {
			if (min_dis[a] == min_dis[b]) return  inds[a]->fitness() > inds[b]->fitness();
			else return min_dis[a] > min_dis[b];
		});
	}




	template<typename TIndivididual>
	void sortByBetterFitnessDis(
		const std::vector<TIndivididual>& inds,
		std::vector<int>& fitness_order, std::vector<int>& parBelong_idx, std::vector<int>& distance_order, ofec::Problem* pro) {
		int size(inds.size());
		parBelong_idx.resize(size);
		fitness_order.resize(size);
		distance_order.resize(size);
		for (int idx(0); idx < size; ++idx) {
			fitness_order[idx] = idx;
			parBelong_idx[idx] = idx;
			distance_order[idx] = idx;
		}
		std::sort(fitness_order.begin(), fitness_order.end(), [&](
			int a, int b
			) {
			if (inds[a]->fitness() == inds[b]->fitness()) return a < b;
			else return inds[a]->fitness() > inds[b]->fitness();
		});


		std::vector<double> min_dis(size, std::numeric_limits<double>::max());
		for (int idx(0); idx < size; ++idx) {
			int cur_idx(fitness_order[idx]);
			auto& sol1(inds[cur_idx]);
			auto& cur_dis(min_dis[cur_idx]);
			auto& belong_idx(parBelong_idx[cur_idx]);
			double dis(0);
			double max_fitness(-std::numeric_limits<double>::max());
			for (int idy(0); idy < idx; ++idy) {
				auto& sol2(inds[fitness_order[idy]]);

				if (sol1->fitness() < sol2->fitness()) {
					dis = sol1->variableDistance(*sol2, pro);
					if (cur_dis > dis) {
						cur_dis = dis;
						belong_idx = fitness_order[idy];
					}
				}

			}
		}
		std::sort(distance_order.begin(), distance_order.end(),
			[&](int a, int b) {
			if (min_dis[a] == min_dis[b]) return  inds[a]->fitness() > inds[b]->fitness();
			else return min_dis[a] > min_dis[b];
		});
	}




	template<typename TIndivididual>
	void sortByFitnessDis(
		const std::vector<TIndivididual>& inds,
		std::vector<int>& fitness_order, std::vector<int>& parBelong_idx, std::vector<int>& distance_order, ofec::Problem* pro) {
		int size(inds.size());
		parBelong_idx.resize(size);
		fitness_order.resize(size);
		distance_order.resize(size);
		for (int idx(0); idx < size; ++idx) {
			fitness_order[idx] = idx;
			parBelong_idx[idx] = idx;
			distance_order[idx] = idx;
		}
		std::sort(fitness_order.begin(), fitness_order.end(), [&](
			int a, int b
			) {
			if (inds[a]->fitness() == inds[b]->fitness()) return a < b;
			else return inds[a]->fitness() > inds[b]->fitness();
		});

		//// for test 
		//std::vector<double> fitness(size, 0);
		//for (int idx(0); idx < size; ++idx) {
		//	fitness[idx] = inds[idx]->fitness();
		//}


		std::vector<double> min_dis(size, std::numeric_limits<double>::max());
		for (int idx(0); idx < size; ++idx) {
			int cur_idx(fitness_order[idx]);
			auto& sol1(inds[cur_idx]);
			auto& cur_dis(min_dis[cur_idx]);
			auto& belong_idx(parBelong_idx[cur_idx]);
			double dis(0);
			for (int idy(0); idy < idx; ++idy) {
				auto& sol2(inds[fitness_order[idy]]);
				/*if (sol1->fitness() < sol2->fitness())*/ {
					dis = sol1->variableDistance(*sol2, pro);
					if (cur_dis > dis) {
						cur_dis = dis;
						belong_idx = fitness_order[idy];
					}
				}

			}
		}
		std::sort(distance_order.begin(), distance_order.end(),
			[&](int a, int b) {
			if (min_dis[a] == min_dis[b]) return a < b;
			else return min_dis[a] > min_dis[b];
		});
	}


	template<typename SolutionType>
	void sortByFitnessDisStaticOrder(
		const std::vector<std::unique_ptr<SolutionType>>& inds,
		const std::vector<int>& fit_order,
		std::vector<int>& order_idxs, std::vector<int>& belong_idx, ofec::Problem* pro) {
		int size(inds.size());
		belong_idx.resize(size);
		order_idxs.resize(size);
		for (int idx(0); idx < size; ++idx) {
			order_idxs[idx] = idx;
			belong_idx[idx] = idx;
		}
		std::vector<double> min_dis(size, std::numeric_limits<double>::max());
		for (int idx(0); idx < size; ++idx) {
			int cur_idx(fit_order[idx]);
			auto& sol1(inds[cur_idx]);
			auto& cur_dis(min_dis[cur_idx]);
			auto& belong_idx(belong_idx[cur_idx]);
			double dis(0);
			for (int idy(0); idy < idx; ++idy) {
				auto& sol2(inds[order_idxs[idy]]);
				dis = sol1->variableDistance(*sol2, pro);
				if (cur_dis > dis) {
					cur_dis = dis;
					belong_idx = order_idxs[idy];
				}
			}
		}
		std::sort(order_idxs.begin(), order_idxs.end(), [&](
			int a, int b
			) {
			if (min_dis[a] == min_dis[b]) return a < b;
			else return min_dis[a] > min_dis[b];
		});
	}


	template<typename SolutionType>
	void sortByFitnessDis(
		const std::vector<std::unique_ptr<SolutionType>>& inds,
		std::vector<int>& order_idxs, std::vector<int>& belong_idx, ofec::Problem* pro) {
		int size(inds.size());
		belong_idx.resize(size);
		order_idxs.resize(size);
		for (int idx(0); idx < size; ++idx) {
			order_idxs[idx] = idx;
			belong_idx[idx] = idx;
		}
		std::sort(order_idxs.begin(), order_idxs.end(), [&](
			int a, int b
			) {
			if (inds[a]->fitness() == inds[b]->fitness()) return a < b;
			else return inds[a]->fitness() > inds[b]->fitness();
		});
		std::vector<double> min_dis(size, std::numeric_limits<double>::max());
		for (int idx(0); idx < size; ++idx) {
			int cur_idx(order_idxs[idx]);
			auto& sol1(inds[cur_idx]);
			auto& cur_dis(min_dis[cur_idx]);
			auto& belong_idx(belong_idx[cur_idx]);
			double dis(0);
			for (int idy(0); idy < idx; ++idy) {
				auto& sol2(inds[order_idxs[idy]]);
				dis = sol1->variableDistance(*sol2, pro);
				if (cur_dis > dis) {
					cur_dis = dis;
					belong_idx = order_idxs[idy];
				}
			}
		}
		std::sort(order_idxs.begin(), order_idxs.end(), [&](
			int a, int b
			) {
			if (min_dis[a] == min_dis[b]) return a < b;
			else return min_dis[a] > min_dis[b];
		});
	}
}

#endif