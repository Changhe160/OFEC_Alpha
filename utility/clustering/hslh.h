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
* heuristic single-linkage hierarchical clustering
*********************************************************************************/
#ifndef HSLHCLUSTERING_H
#define HSLHCLUSTERING_H

#include "group.h"

#include<utility>
#include<vector>

#include"../../core/algorithm/population.h"		//

namespace OFEC {
	template<typename Individual >
	class HSLH {
		std::vector<group<Individual> > m_group;
		std::vector<std::vector<real> > m_dis;	//distance between all objects

		real m_inter_dis;			// inter distance of all group
		real m_intra_dis;			// intra distance of all group
		int m_space;				//objective space or solution space

		typedef typename std::vector<group<Individual>>::iterator mem_iter;

	public:
		HSLH(population<Individual> &p, int space = 0) :m_space(space), m_group(p.size()), m_dis(p.size(), std::vector<real>(p.size())) {	//construct function	
			for (int k = 0; k < p.size(); ++k) {
				m_group[k].initialize(std::make_pair(&p[k], k));
			}

			for (int i = 0; i < p.size(); i++) {					//initialize m_dis
				m_dis[i][i] = 0;
				for (int j = 0; j < i; j++) {
					if (m_space == 0)
						m_dis[i][j] = m_dis[j][i] = m_group[i][0].first->variable_distance(*m_group[j][0].first);
					else
						m_dis[i][j] = m_dis[j][i] = m_group[i][0].first->objective_distance(*m_group[j][0].first);
				}
			}
			update_distance();

		}

		void update_distance() {
			m_inter_dis = m_intra_dis = 0;
			//calculate inter-distance
			for (unsigned i = 0; i < m_group.size(); i++) {
				for (unsigned j = 0; j < i; j++) {
					m_inter_dis += m_dis[m_group[i].best()][m_group[j].best()];
				}
			}
		
			m_inter_dis /= m_group.size()*(m_group.size() - 1) / 2;		//average inter distance

			// calulate intra-distance
			for (unsigned i = 0; i < m_group.size(); i++) {
				m_intra_dis += m_group[i].intra_dis();
			}

			m_intra_dis /= m_group.size();				//average intra distance

		}
		
		void clustering(int minsize) {			
			while (1) {
				mem_iter i = m_group.begin();
				while (i != m_group.end() && i->size() > 1) i++;
				if (i == m_group.end()) 
					break;

				auto p = nearest_group(-1);		//find nearest two group

				if ((p.first == m_group.end())) 
					break;

				p.first->merge(*p.second,m_dis);
				m_group.erase(p.second);

				update_distance();				//update distance

				if (m_inter_dis <= m_intra_dis)
					break;

			}

		}

		void clustering(int maxsize, int minsize) {			
			while (1) {
				mem_iter i = m_group.begin();
				while (i != m_group.end() && i->size() > 1) 
					i++;
				if (i == m_group.end()) 
					break;

				auto &&p = nearest_group(maxsize);		//find nearest two group

				if ((p.first == m_group.end()) ) 
					break;
				p.first->merge(*p.second,m_dis);
				m_group.erase(p.second);

				update_distance();				//update distance

				if (m_inter_dis <= m_intra_dis)
					break;

			}
		}

		group<Individual>& operator[](const int i) {
			return m_group[i];
		}
		int size() {					//size of group
			return m_group.size();
		}
		int size_above(int n) {
			int count = 0;
			for (auto i = m_group.begin(); i != m_group.end(); ++i) {
				if (i->size() > n) ++count;
			}
			return count;
		}

		std::pair<mem_iter, mem_iter> nearest_group(int maxsize) {
			
			real Min_dis = std::numeric_limits<real>::max(), dist;
			auto g1 = m_group.end(), g2 = m_group.end();

			for (auto i = m_group.begin(); i != m_group.end(); ++i) {
				// can't merge two mp_groups whose total m_number are greater than m_maxsize
				for (auto j = i+1; j != m_group.end(); ++j) {		
					if (maxsize > 0 && (i->size() + j->size()) > maxsize) continue;

					dist = m_dis[i->best()][j->best()];
					if (Min_dis > dist) {
						Min_dis = dist;
						g1 = i;
						g2 = j;
					}
				}
			}

			return std::make_pair(g1, g2);
		}
	};



}

#endif // CLUSTERING_H
