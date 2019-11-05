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
* a cluser of individuals
*********************************************************************************/
#ifndef GROUP_H
#define GROUP_H
#include<vector>
#include<memory>
#include"../functional.h"

#include"../../core/definition.h"		//

namespace OFEC {
	template <typename Individual>
	class group {
		std::vector <std::pair<Individual *, int> > m_member;
		int m_id = 0, m_best = 0;
		real m_intra_dis = 0;			//average distance of intra

	public:
		group() = default;
		void initialize(const std::pair<Individual *, int>  &item) {
			m_member.push_back(item);
			m_best = item.second;
		}
		void merge(group &g, std::vector<std::vector<real>> &dis) {
			for (int i = 0; i < g.size(); ++i) {	//update member
				m_member.push_back(g.m_member[i]);
			}

			update_best(g);					//update best

			update_distance(dis);			//update average distance of intra
		}

		int id() {						//id of best individual in group
			return m_id;				
		}

		int best() {					//id of best individual in population
			return m_best;
		}

		int size()const {
			return m_member.size();
		}

		real intra_dis() {
			return m_intra_dis;
		}

		std::pair<Individual*, int>& operator[](size_t i) {
			return m_member[i];
		}
		const std::pair<Individual*, int>& operator[](size_t i)const {
			return m_member[i];
		}

		void update_best(group &g) {	//update best
			
			dominationship r = objective_compare(m_member[m_best].first->objective(), 
				g.m_member[g.m_best].first->objective(), global::ms_global->m_problem->opt_mode());

			if (dominationship::Dominated == r) {
				m_best = g.m_best;
				m_id = m_member.size() - g.size() + g.id();
			}
				
			else if (dominationship::Non_dominated==r){
				int rank[2] = {0,0};
				
				if (global::ms_global->m_problem->opt_mode()[0] == optimization_mode::Minimization) {
					for (int i = 0; i < m_member[0].first->objective_size(); ++i) {
						if (m_member[m_best].first->objective(i) < m_member[g.m_best].first->objective(i))
							rank[0]++;
						else if (m_member[m_best].first->objective(i) > m_member[g.m_best].first->objective(i))
							rank[1]++;
					}
				}
				else {
					for (int i = 0; i < m_member[0].first->objective_size(); ++i) {
						if (m_member[m_best].first->objective(i) > m_member[g.m_best].first->objective(i))
							rank[0]++;
						else if (m_member[m_best].first->objective(i) < m_member[g.m_best].first->objective(i))
							rank[1]++;
					}
				}
				
				if (rank[0] < rank[1]) {
					m_best = g.m_best;
					m_id = m_member.size() - g.size() + g.id();
				}				
			}
		}

		void update_distance(const std::vector<std::vector<real>> &dis) {				//update average distance of intra
			real sum = 0;
			for (int i = 0; i < m_member.size(); ++i) {
				for (int j = 0; j < i; ++j) {
					sum += dis[m_member[i].second][m_member[j].second];
				}
			}
			int c = m_member.size()*(m_member.size() - 1) / 2;
			m_intra_dis = sum / c;
		}
	};
}
#endif // GROUP_H
