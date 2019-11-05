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

*  See the details of MOEA/D-DE in the following paper
*  H. Li and Q. Zhang, Comparison Between NSGA-II and MOEA/D on a Set of Multiobjective Optimization
*  Problems with Complicated Pareto Sets, Technical Report CES-476, Department of Computer Science,
*  University of Essex, 2009
*************************************************************************/
// Created: 30 December 2014
// Last modified: 18 July 2019 by Xiaofang Wu (email:wuxiaofang@cug.edu.cn)

#ifndef MOEA_DE_H
#define MOEA_DE_H
#include "../population.h"
#include "../individual.h"
#include <numeric>

namespace OFEC {
	namespace DE {
		template<typename Individual = DE::individual>
		class MOEA_DE_pop : public DE::population<Individual> {
		public:				
			MOEA_DE_pop(size_t size_pop);
			MOEA_DE_pop();
			~MOEA_DE_pop() {}
			MOEA_DE_pop<Individual>& operator=(const MOEA_DE_pop &rhs);
			void cross_mutate(const std::vector<size_t> &index, Individual &child);
			void diff_evo_xover(const Individual &parent1, const Individual &parent2, const Individual &parent3, Individual &child);
			void real_mutation(Individual &child, real r, real etam);
			size_t tournament_selection(size_t tournament_size = 2);
		protected:
			real m_r;
			real m_etam;
			std::vector<size_t> m_rand_seq; // Random sequence of the population
		};

		template<typename Individual>
		MOEA_DE_pop<Individual>::MOEA_DE_pop(size_t size_pop) : population<Individual>(size_pop), m_etam(20) {
			//m_F = 0.5;
			int numDim = global::ms_global->m_problem->variable_size();
			m_r = 1.0 / numDim;
			m_rand_seq.resize(size_pop);
			std::iota(m_rand_seq.begin(), m_rand_seq.end(), 0);
		}

		template<typename Individual>
		MOEA_DE_pop<Individual>::MOEA_DE_pop() : population<>(), m_etam(20) {
			int numDim = global::ms_global->m_problem->variable_size();
			m_r = 1.0 / numDim;
		}
		template<typename Individual>
		MOEA_DE_pop<Individual>& MOEA_DE_pop<Individual>::operator=(const MOEA_DE_pop &rhs) {
			this->m_pop.clear();
			this->m_iter = rhs.m_iter;
			this->m_id = rhs.m_id;
			for (size_t i = 0; i < rhs.size(); i++) {
				this->m_pop.push_back(std::unique_ptr<Individual>(new Individual(rhs[i])));
			}

			this->m_order = rhs.m_order;
			this->m_ordered = rhs.m_ordered;
			this->m_best_updated = rhs.m_best_updated;
			this->m_worst_updated = rhs.m_worst_updated;
			//m_max_id = rhs.m_max_id;
			this->m_best_monitor = rhs.m_best_monitor;
			this->m_worst_monitor = rhs.m_worst_monitor;

			/*update_best();
			update_worst();*/
			return *this;
		}

		template<typename Individual>
		void MOEA_DE_pop<Individual>::cross_mutate(const std::vector<size_t> &index, Individual &child) {
			diff_evo_xover(*this->m_inds[index[0]], *this->m_inds[index[1]], *this->m_inds[index[2]], child);
			real_mutation(child, m_r, m_etam);
			//child.evaluate();
		}

		template<typename Individual>
		void MOEA_DE_pop<Individual>::diff_evo_xover(const Individual &parent1, const Individual &parent2, const Individual &parent3, Individual &child) {
			int numDim = global::ms_global->m_problem->variable_size();
			int idx_rnd = global::ms_global->m_uniform[caller::Algorithm]->next_non_standard(0, numDim);
			auto boun = CONTINUOUS_CAST->range();
			real rate = 0.5;
			child = parent1;
			for (int n = 0; n < numDim; n++) {
				/*Selected Two Parents*/
				child.variable()[n] = parent1.variable()[n] + rate * (parent3.variable()[n] - parent2.variable()[n]);

				if (child.variable()[n] < boun[n].limit.first) {
					real rnd = global::ms_global->m_uniform[caller::Algorithm]->next();
					child.variable()[n] = boun[n].limit.first + rnd * (parent1.variable()[n] - boun[n].limit.first);
				}
				if (child.variable()[n] > boun[n].limit.second) {
					real rnd = global::ms_global->m_uniform[caller::Algorithm]->next();
					child.variable()[n] = boun[n].limit.second - rnd * (boun[n].limit.second - parent1.variable()[n]);
				}
			}

		}

		template<typename Individual>
		void MOEA_DE_pop<Individual>::real_mutation(Individual &child, real r, real etam) {
			int numDim = global::ms_global->m_problem->variable_size();
			auto boun = CONTINUOUS_CAST->range();

			real rnd, delta1, delta2, mut_pow, deltaq;
			real y, yl, yu, val, xy;
			real eta_m = etam;

			int  id_rnd = global::ms_global->m_uniform[caller::Algorithm]->next_non_standard(0, numDim);

			for (int j = 0; j < numDim; j++)
			{
				if (global::ms_global->m_uniform[caller::Algorithm]->next() <= r)
				{
					y = child.variable()[j];
					yl = boun[j].limit.first;
					yu = boun[j].limit.second;
					delta1 = (y - yl) / (yu - yl);
					delta2 = (yu - y) / (yu - yl);
					rnd = global::ms_global->m_uniform[caller::Algorithm]->next();
					mut_pow = 1.0 / (eta_m + 1.0);
					if (rnd <= 0.5)
					{
						xy = 1.0 - delta1;
						val = 2.0*rnd + (1.0 - 2.0*rnd)*(pow(xy, (eta_m + 1.0)));
						deltaq = pow(val, mut_pow) - 1.0;
					}
					else
					{
						xy = 1.0 - delta2;
						val = 2.0*(1.0 - rnd) + 2.0*(rnd - 0.5)*(pow(xy, (eta_m + 1.0)));
						deltaq = 1.0 - (pow(val, mut_pow));
					}
					y = y + deltaq * (yu - yl);
					if (y < yl)
						y = yl;
					if (y > yu)
						y = yu;
					child.variable()[j] = y;
				}
			}
		}

		template<typename Individual>
		size_t MOEA_DE_pop<Individual>::tournament_selection(size_t tournament_size) {
			global::ms_global->m_uniform[caller::Algorithm]->shuffle(this->m_rand_seq.begin(), this->m_rand_seq.end());
			size_t idx_best = this->m_rand_seq[0];
			for (size_t k = 1; k < tournament_size; ++k)
				if (this->m_inds[this->m_rand_seq[k]]->dominate(*this->m_inds[idx_best]))
					idx_best = this->m_rand_seq[k];
			return idx_best;
		}
	}
}

#endif //MOEA_DE_H