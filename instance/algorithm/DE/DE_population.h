/******************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*******************************************************************************
* Author: Changhe Li and Li Zhou
* Email: changhe.lw@gmail.com, 441837060@qq.com
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
* class DEpopulation manages a set of DEindividuals.
*********************************************************************************/
// updated Mar 28, 2018 by Li Zhou

/*Storn, R. and Price, K. (1997), "Differential Evolution - A Simple and Efficient Heuristic for Global Optimization over Continuous Spaces",
Journal of Global Optimization, 11, pp. 341-359*/
#ifndef OFEC_DEPOPULATION_H
#define OFEC_DEPOPULATION_H
#include "../../../core/algorithm/population.h"

namespace OFEC {
	namespace DE {
		template< typename Individual >
		class population : public OFEC::population<Individual> {
		protected:
			double  m_F, m_CR;
			enum DE_mutation_stratgy { DE_rand_1, DE_best_1, DE_target_to_best_1, DE_best_2, DE_rand_2, DE_rand_to_best_1, DE_target_to_rand_1 };
			DE_mutation_stratgy m_mutation_strategy;
		public:
			using population_type = OFEC::population<Individual>;

			population() = default;
			template<typename ... Args>
			population(size_t no, Args&& ... args);
			population(const population &rhs);
			population(population&&rhs);

			population& operator=(const population &rhs);
			population& operator=(population&&rhs);

			void set_mutation_strategy(DE_mutation_stratgy rS);
			virtual void mutate(const int idx);
			void set_parmeter(double cr, double f);
			void default_parameter();
			virtual void reinitialize();
			void mutate(int idx, const std::vector<int>&var);
		protected:
			virtual void select_in_neighborhood(int number, std::vector<int>&, std::vector<int>&);
			virtual void select(int base, int number, std::vector<int>& result);
			evaluation_tag evolve();

		};

		template< typename Individual>
		template<typename ... Args>
		population<Individual>::population(size_t no, Args&& ... args) : population_type(no, std::forward<Args>(args)...) {
			default_parameter();
		}
		template< typename Individual >
		population<Individual>::population(const population &rhs) : population_type(rhs), m_F(rhs.m_F), m_CR(rhs.m_CR), m_mutation_strategy(rhs.m_mutation_strategy) {

		}
		template< typename Individual >
		population<Individual>::population(population&&rhs) : population_type(std::move(rhs)), m_F(std::move(rhs.m_F)), m_CR(std::move(rhs.m_CR)), \
			m_mutation_strategy(std::move(rhs.m_mutation_strategy)) {

		}

		template< typename Individual >
		void population<Individual>::set_mutation_strategy(DE_mutation_stratgy rS) {
			m_mutation_strategy = rS;
		}
		template< typename Individual >
		population<Individual> & population<Individual>::operator=(const population & rhs) {
			if (this == &rhs) return *this;
			OFEC::population<Individual>::operator=(rhs);
			m_CR = rhs.m_CR;
			m_F = rhs.m_F;
			m_mutation_strategy = rhs.m_mutation_strategy;
			return *this;
		}
		template< typename Individual >
		population<Individual> & population<Individual>::operator=(population&& rhs) {
			if (this == &rhs) return *this;
			OFEC::population<Individual>::operator=(std::move(rhs));
			m_CR = rhs.m_CR;
			m_F = rhs.m_F;
			m_mutation_strategy = std::move(rhs.m_mutation_strategy);
			return *this;
		}
		template< typename Individual >
		void population<Individual>::mutate(const int idx) {
			std::vector<int> ridx;
			switch (m_mutation_strategy) {
			case DE_rand_1:
				select(idx, 3, ridx);
				this->m_pop[idx]->mutate(m_F, m_pop[ridx[0]].get(), m_pop[ridx[1]].get(), m_pop[ridx[2]].get());
				break;
			case DE_best_1:
				select(idx, 2, ridx);
				this->m_pop[idx]->mutate(m_F, m_arc[0].get(), m_pop[ridx[0]].get(), m_pop[ridx[1]].get());
				break;
			case DE_target_to_best_1:
				select(idx, 2, ridx);
				this->m_pop[idx]->mutate(m_F, m_pop[idx].get(), m_arc[0].get(), m_pop[idx].get(), m_pop[ridx[0]].get(), m_pop[ridx[1]].get());
				break;
			case DE_best_2:
				select(idx, 4, ridx);
				this->m_pop[idx]->mutate(m_F, m_arc[0].get(), m_pop[ridx[0]].get(), m_pop[ridx[1]].get(), m_pop[ridx[2]].get(), m_pop[ridx[3]].get());
				break;
			case DE_rand_2:
				select(idx, 5, ridx);
				this->m_pop[idx]->mutate(m_F, m_pop[ridx[0]].get(), m_pop[ridx[1]].get(), m_pop[ridx[2]].get(), m_pop[ridx[3]].get(), m_pop[ridx[4]].get());
				break;
			case DE_rand_to_best_1:
				select(idx, 3, ridx);
				this->m_pop[idx]->mutate(m_F, m_pop[ridx[0]].get(), m_arc[0].get(), m_pop[ridx[0]].get(), m_pop[ridx[1]].get(), m_pop[ridx[2]].get());
				break;
			case DE_target_to_rand_1:
				select(idx, 3, ridx);
				this->m_pop[idx]->mutate(m_F, m_pop[idx].get(), m_pop[ridx[0]].get(), m_pop[idx].get(), m_pop[ridx[1]].get(), m_pop[ridx[2]].get());
				break;
			}
		}
		template< typename Individual >
		void population<Individual>::mutate(int idx, const std::vector<int>&var) {
			std::vector<int> ridx;
			switch (m_mutation_strategy) {
			case DE_rand_1:
				select(idx, 3, ridx);
				this->m_pop[idx]->mutate(m_F, var, m_pop[ridx[0]], m_pop[ridx[1]], m_pop[ridx[2]]);
				break;
			case DE_best_1:
				select(idx, 2, ridx);
				this->m_pop[idx]->mutate(m_F, var, m_arc[0], m_pop[ridx[0]], m_pop[ridx[1]]);
				break;
			case DE_target_to_best_1:
				select(idx, 2, ridx);
				this->m_pop[idx]->mutate(m_F, var, m_pop[idx], m_arc[0], m_pop[idx], m_pop[ridx[0]], m_pop[ridx[1]]);
				break;
			case DE_best_2:
				select(idx, 4, ridx);
				this->m_pop[idx]->mutate(m_F, var, m_arc[0], m_pop[ridx[0]], m_pop[ridx[1]], m_pop[ridx[2]], m_pop[ridx[3]]);
				break;
			case DE_rand_2:
				select(idx, 5, ridx);
				this->m_pop[idx]->mutate(m_F, var, m_pop[ridx[0]], m_pop[ridx[1]], m_pop[ridx[2]], m_pop[ridx[3]], m_pop[ridx[4]]);
				break;
			case DE_rand_to_best_1:
				select(idx, 3, ridx);
				this->m_pop[idx]->mutate(m_F, var, m_pop[ridx[0]], m_arc[0], m_pop[ridx[0]], m_pop[ridx[1]], m_pop[ridx[2]]);
				break;
			case DE_target_to_rand_1:
				select(idx, 3, ridx);
				this->m_pop[idx]->mutate(m_F, var, m_pop[idx], m_pop[ridx[0]], m_pop[idx], m_pop[ridx[1]], m_pop[ridx[2]]);
				break;
			}
		}
		template< typename Individual >
		evaluation_tag population<Individual>::evolve() {
			if (this->m_pop.size() < 5) {
				throw myexcept("the population size cannot be smaller than 5@DE::population<Individual>::evolve()");
			}
			evaluation_tag tag = evaluation_tag::Normal;

			for (size_t i = 0; i < this->m_pop.size(); ++i) {
				mutate(i);
				this->m_pop[i]->recombine(m_CR);
			}

			//this->updateIDnIndex();
			for (size_t i = 0; i < this->m_pop.size(); ++i) {
				tag = this->m_pop[i]->select();
				this->update_archive(this->m_pop[i]);
				if (tag != evaluation_tag::Normal) break;

			}

			if (tag == evaluation_tag::Normal) {
				//this->m_center = *this->m_best[0];
				//this->updateCurRadius();
				this->m_iter++;
			}
			return tag;
		}
		template< typename Individual >
		void population<Individual>::set_parmeter(const double cr, const double f) {
			m_CR = cr;
			m_F = f;
		}
		template< typename Individual >
		void population<Individual>::default_parameter() {
			m_CR = 0.6;
			m_F = 0.5;
			m_mutation_strategy = DE_best_1;
		}
		template< typename Individual >
		void population<Individual>::reinitialize() {
			for (size_t i = 0; i < this->m_pop.size(); ++i) {
				this->m_pop[i]->initialize(i);
				//this->m_pop[i]->m_flag = true;
			}
			if (this->terminating()) return;
			this->update_best();

			for (size_t i = 0; i < this->m_best.size(); ++i) {
				this->update_archive(*m_best[i]);
			}
		}
		template< typename Individual >
		void population<Individual>::select(int base, int number, std::vector<int>& result) {
			std::vector<int> candidate;
			for (int i = 0; i < this->m_pop.size(); ++i) {
				if (base != i) candidate.push_back(i);
			}
			result.resize(number);
			for (int i = 0; i < number; ++i) {
				int idx = global::ms_global->m_uniform[caller::Algorithm]->next_non_standard<int>(0, (int)candidate.size() - i);
				result[i] = candidate[idx];
				if (idx != candidate.size() - (i + 1)) candidate[idx] = candidate[candidate.size() - (i + 1)];
			}
		}
		template< typename Individual >
		void population<Individual>::select_in_neighborhood(int number, std::vector<int>& candidate, std::vector<int>& result) {
			for (int i = 0; i < number; ++i) {
				int idx = global::ms_global->m_uniform[caller::Algorithm]->next_non_standard<int>(0, (int)candidate.size() - i);
				result[i] = candidate[idx];
				if (idx != candidate.size() - (i + 1)) candidate[idx] = candidate[candidate.size() - (i + 1)];
			}
		}
	}
}
#endif // !OFEC_DEPOPULATION_H


