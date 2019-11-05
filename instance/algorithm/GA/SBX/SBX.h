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
// Created: 7 Jan 2015
// Last modified: 7 Oct 2018 by Junchen Wang (email:wangjunchen.chris@gmail.com)


/*-----------------------------------------------------------------------------------
   SimulatedBinaryCrossover : simulated binary crossover(SBX)
   The implementation was adapted from the code of function realcross() in crossover.c
   http://www.iitk.ac.in/kangal/codes/nsga2/nsga2-gnuplot-v1.1.6.tar.gz
   ref: http://www.slideshare.net/paskorn/simulated-binary-crossover-presentation#
-----------------------------------------------------------------------------------*/

#ifndef OFEC_SBX_H
#define OFEC_SBX_H

#include "../population.h"
#include "../../../../core/problem/continuous/continuous.h"
#include <algorithm>

namespace OFEC {
	template<typename Individual = individual<>>
	class SBX_pop : public GA::population<Individual> {
	protected:
		real m_ceta = 0; // crossover eta
		real m_meta = 0; // mutation eta
	public:
		SBX_pop(size_t size_pop);
		void set_eta(real ceta, real meta) { m_ceta = ceta; m_meta = meta; }
	protected:
		void crossover(size_t idx_parent1, size_t idx_parent2, Individual& child1, Individual& child2) override;
		void mutate(Individual &indv) override;
	private:
		real get_betaq(real rand, real alpha);
	};

	template<typename Individual>
	SBX_pop<Individual>::SBX_pop(size_t size_pop) : GA::population<Individual>(size_pop, CONTINUOUS_CAST->variable_size()) {}

	template<typename Individual>
	void SBX_pop<Individual>::crossover(size_t idx_parent1, size_t idx_parent2, Individual& child1, Individual& child2) {
		child1 = *this->m_inds[idx_parent1];
		child2 = *this->m_inds[idx_parent2];

		if (global::ms_global->m_uniform[caller::Algorithm]->next() > this->m_cr) return; // not crossovered

		variable_vector<real> &c1 = child1.variable(), &c2 = child2.variable();
		const variable_vector<real> &p1 = this->m_inds[idx_parent1]->variable(), &p2 = this->m_inds[idx_parent2]->variable();

		for (size_t i = 0; i < c1.size(); i += 1) {
			if (global::ms_global->m_uniform[caller::Algorithm]->next() > 0.5) continue; // these two variables are not crossovered
			if (std::fabs(static_cast<real>(p1[i]) - static_cast<real>(p2[i])) <= 1.0e-14) continue; // two values are the same

			real y1 = std::min(p1[i], p2[i]), y2 = std::max(p1[i], p2[i]);

			real lb = CONTINUOUS_CAST->range(i).first, ub = CONTINUOUS_CAST->range(i).second;

			real rand = global::ms_global->m_uniform[caller::Algorithm]->next();

			// child 1
			real beta = 1.0 + (2.0*(y1 - lb) / (y2 - y1)),
				alpha = 2.0 - pow(beta, -(m_ceta + 1.0));
			real betaq = get_betaq(rand, alpha);

			c1[i] = 0.5*((y1 + y2) - betaq * (y2 - y1));

			// child 2
			beta = 1.0 + (2.0*(ub - y2) / (y2 - y1));
			alpha = 2.0 - pow(beta, -(m_ceta + 1.0));
			betaq = get_betaq(rand, alpha);

			c2[i] = 0.5*((y1 + y2) + betaq * (y2 - y1));

			// boundary checking
			c1[i] = std::min(ub, std::max(lb, static_cast<real>(c1[i])));
			c2[i] = std::min(ub, std::max(lb, static_cast<real>(c2[i])));

			if (global::ms_global->m_uniform[caller::Algorithm]->next() <= 0.5)
				std::swap(c1[i], c2[i]);
		}
	}

	template<typename Individual>
	void SBX_pop<Individual>::mutate(Individual &indv) {
		variable_vector<real> &x = indv.variable();		

		for (size_t i = 0; i < x.size(); i += 1) {
			if (global::ms_global->m_uniform[caller::Algorithm]->next() <= this->m_mr) {
				real y = indv.variable()[i];
				real lb = CONTINUOUS_CAST->range(i).first, ub = CONTINUOUS_CAST->range(i).second;
				real delta1 = (y - lb) / (ub - lb), delta2 = (ub - y) / (ub - lb);
				real mut_pow = 1.0 / (m_meta + 1.0);
				real rnd = global::ms_global->m_uniform[caller::Algorithm]->next(), deltaq;
				if (rnd <= 0.5) {
					real xy = 1.0 - delta1;
					real val = 2.0*rnd + (1.0 - 2.0*rnd)*(pow(xy, (m_meta + 1.0)));
					deltaq = pow(val, mut_pow) - 1.0;
				}
				else {
					real xy = 1.0 - delta2;
					real val = 2.0*(1.0 - rnd) + 2.0*(rnd - 0.5)*(pow(xy, (m_meta + 1.0)));
					deltaq = 1.0 - (pow(val, mut_pow));
				}
				y = y + deltaq * (ub - lb);
				y = std::min(ub, std::max(lb, y));
				x[i] = y;
			}
		}
	}

	template<typename Individual>
	real SBX_pop<Individual>::get_betaq(real rand, real alpha) {
		real betaq;
		if (rand <= (1.0 / alpha))
			betaq = std::pow((rand*alpha), (1.0 / (m_ceta + 1.0)));
		else
			betaq = std::pow((1.0 / (2.0 - rand * alpha)), (1.0 / (m_ceta + 1.0)));
		return betaq;
	}
	


}

#endif // !OFEC_SBX_H
