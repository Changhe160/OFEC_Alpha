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
#ifndef DE_POPULATION_H
#define DE_POPULATION_H
#include "../../../core/algorithm/population.h"
#include "individual.h"

namespace OFEC {
	namespace DE {
		enum class mutation_strategy { rand_1, best_1, target_to_best_1, best_2, rand_2, rand_to_best_1, target_to_rand_1, nrand_1 };
		template<typename Individual = individual>
		class population : public OFEC::population<Individual> {
		protected:
			real  m_F = 0, m_CR = 0;
			mutation_strategy m_mutation_strategy = mutation_strategy::rand_1;
			recombine_strategy m_recombine_strategy = recombine_strategy::binomial;
		public:
			using population_type = OFEC::population<Individual>;
			using population_type::m_inds;
			using population_type::m_best;
			using population_type::m_iter;
			using population_type::update_best;
			population()= default;
			population(size_t size_pop);
			population(const population &rhs);
			population(population&&rhs) noexcept;

			population& operator=(const population &rhs);
			population& operator=(population&&rhs) noexcept;

			void set_mutation_strategy(mutation_strategy rS);
			virtual void mutate(const int idx);
			void recombine(int idx);
			void set_parameter(real cr, real f);
			void default_parameter();
			void mutate(int idx, const std::vector<int>&var);
			recombine_strategy get_recombine_strategy() { return m_recombine_strategy; }
			evaluation_tag evolve() override;
			virtual void select_in_neighborhood(int number, std::vector<size_t>, std::vector<size_t>&);
			virtual void select(int base_, int number, std::vector<size_t>& result);
			real& F() { return m_F; }
			real& CR() { return m_CR; }
		};

		template<typename Individual>
		population<Individual>::population(size_t size_pop) : population_type(size_pop, global::ms_global->m_problem->variable_size()) {
		}
		template<typename Individual>
		population<Individual>::population(const population &rhs) : population_type(rhs), m_F(rhs.m_F), m_CR(rhs.m_CR), m_mutation_strategy(rhs.m_mutation_strategy) {
		}
		template<typename Individual>
		population<Individual>::population(population &&rhs) noexcept : population_type(std::move(rhs)), m_F(std::move(rhs.m_F)), m_CR(std::move(rhs.m_CR)), \
			m_mutation_strategy(std::move(rhs.m_mutation_strategy)) {
		}

		template<typename Individual>
		void population<Individual>::set_mutation_strategy(mutation_strategy rS) {
			m_mutation_strategy = rS;
		}
		template<typename Individual>
		population<Individual> & population<Individual>::operator=(const population & rhs) {
			if (this == &rhs) return *this;
			population_type::operator=(rhs);
			m_CR = rhs.m_CR;
			m_F = rhs.m_F;
			m_mutation_strategy = rhs.m_mutation_strategy;
			return *this;
		}
		template<typename Individual>
		population<Individual> & population<Individual>::operator=(population&& rhs) noexcept
		{
			if (this == &rhs) return *this;
			population_type::operator=(std::move(rhs));
			m_CR = rhs.m_CR;
			m_F = rhs.m_F;
			m_mutation_strategy = std::move(rhs.m_mutation_strategy);
			return *this;
		}
		template<typename Individual>
		void population<Individual>::mutate(const int idx) {
			std::vector<size_t> ridx;
			switch (m_mutation_strategy) {
			case mutation_strategy::rand_1:
				select(idx, 3, ridx);
				m_inds[idx]->mutate(m_F, m_inds[ridx[0]].get(), m_inds[ridx[1]].get(), m_inds[ridx[2]].get());
				break;
			case mutation_strategy::best_1:
				select(idx, 2, ridx);
				m_inds[idx]->mutate(m_F, m_best[0].get(), m_inds[ridx[0]].get(), m_inds[ridx[1]].get());
				break;
			case mutation_strategy::target_to_best_1:
				select(idx, 2, ridx);
				m_inds[idx]->mutate(m_F, m_inds[idx].get(), m_best[0].get(), m_inds[idx].get(), m_inds[ridx[0]].get(), m_inds[ridx[1]].get());
				break;
			case mutation_strategy::best_2:
				select(idx, 4, ridx);
				m_inds[idx]->mutate(m_F, m_best[0].get(), m_inds[ridx[0]].get(), m_inds[ridx[1]].get(), m_inds[ridx[2]].get(), m_inds[ridx[3]].get());
				break;
			case mutation_strategy::rand_2:
				select(idx, 5, ridx);
				m_inds[idx]->mutate(m_F, m_inds[ridx[0]].get(), m_inds[ridx[1]].get(), m_inds[ridx[2]].get(), m_inds[ridx[3]].get(), m_inds[ridx[4]].get());
				break;
			case mutation_strategy::rand_to_best_1:
				select(idx, 3, ridx);
				m_inds[idx]->mutate(m_F, m_inds[ridx[0]].get(), m_best[0].get(), m_inds[ridx[0]].get(), m_inds[ridx[1]].get(), m_inds[ridx[2]].get());
				break;
			case mutation_strategy::target_to_rand_1:
				select(idx, 3, ridx);
				m_inds[idx]->mutate(m_F, m_inds[idx].get(), m_inds[ridx[0]].get(), m_inds[idx].get(), m_inds[ridx[1]].get(), m_inds[ridx[2]].get());
				break;
			case mutation_strategy::nrand_1:
				int nearest = this->nearest_neighbour(idx, 1).begin()->second;
				select(idx, 2, ridx);
				m_inds[idx]->mutate(m_F, m_inds[nearest].get(), m_inds[ridx[0]].get(), m_inds[ridx[1]].get());
				break;
			}
		}
		
		template<typename Individual>
		void population<Individual>::recombine(int idx) {
			m_inds[idx]->recombine(m_CR, m_recombine_strategy);
		}

		template<typename Individual>
		void population<Individual>::mutate(int idx, const std::vector<int>&var) {
			std::vector<size_t> ridx;
			switch (m_mutation_strategy) {
			case mutation_strategy::rand_1:
				select(idx, 3, ridx);
				m_inds[idx]->mutate(m_F, var, m_inds[ridx[0]].get(), m_inds[ridx[1]].get(), m_inds[ridx[2]].get());
				break;
			case mutation_strategy::best_1:
				select(idx, 2, ridx);
				m_inds[idx]->mutate(m_F, var, m_best[0].get(), m_inds[ridx[0]].get(), m_inds[ridx[1]].get());
				break;
			case mutation_strategy::target_to_best_1:
				select(idx, 2, ridx);
				m_inds[idx]->mutate(m_F, var, m_inds[idx].get(), m_best[0].get(), m_inds[idx].get(), m_inds[ridx[0]].get(), m_inds[ridx[1]].get());
				break;
			case mutation_strategy::best_2:
				select(idx, 4, ridx);
				m_inds[idx]->mutate(m_F, var, m_best[0].get(), m_inds[ridx[0]].get(), m_inds[ridx[1]].get(), m_inds[ridx[2]].get(), m_inds[ridx[3]].get());
				break;
			case mutation_strategy::rand_2:
				select(idx, 5, ridx);
				m_inds[idx]->mutate(m_F, var, m_inds[ridx[0]].get(), m_inds[ridx[1]].get(), m_inds[ridx[2]].get(), m_inds[ridx[3]].get(), m_inds[ridx[4]].get());
				break;
			case mutation_strategy::rand_to_best_1:
				select(idx, 3, ridx);
				m_inds[idx]->mutate(m_F, var, m_inds[ridx[0]].get(), m_best[0].get(), m_inds[ridx[0]].get(), m_inds[ridx[1]].get(), m_inds[ridx[2]].get());
				break;
			case mutation_strategy::target_to_rand_1:
				select(idx, 3, ridx);
				m_inds[idx]->mutate(m_F, var, m_inds[idx].get(), m_inds[ridx[0]].get(), m_inds[idx].get(), m_inds[ridx[1]].get(), m_inds[ridx[2]].get());
				break;
			case mutation_strategy::nrand_1:
				int nearest = this->nearest_neighbour(idx, 1).begin()->second;
				select(idx, 2, ridx);
				m_inds[idx]->mutate(m_F, m_inds[nearest].get(), m_inds[ridx[0]].get(), m_inds[ridx[1]].get());
				break;
			}
		}
		template<typename Individual>
		evaluation_tag population<Individual>::evolve() {
			if (m_inds.size() < 5) {
				throw myexcept("the population size cannot be smaller than 5@DE::population<Individual>::evolve()");
			}
			evaluation_tag tag = evaluation_tag::Normal;

			if (m_mutation_strategy == mutation_strategy::best_1 ||
				m_mutation_strategy == mutation_strategy::target_to_best_1 ||
				m_mutation_strategy == mutation_strategy::best_2 ||
				m_mutation_strategy == mutation_strategy::rand_to_best_1)
				update_best();

			for (size_t i = 0; i < m_inds.size(); ++i) {
				mutate(i);
				recombine(i);
				tag = m_inds[i]->select();
				if (tag != evaluation_tag::Normal) break;
			}

			if (tag == evaluation_tag::Normal)
				m_iter++;
			return tag;
		}
		template<typename Individual>
		void population<Individual>::set_parameter(const real cr, const real f) {
			m_CR = cr;
			m_F = f;
		}
		template<typename Individual>
		void population<Individual>::default_parameter() {
			m_CR = 0.6;
			m_F = 0.5;
			m_mutation_strategy = mutation_strategy::best_1;
			m_recombine_strategy = recombine_strategy::binomial;
		}

		template<typename Individual>
		void population<Individual>::select(int base_, int number, std::vector<size_t>& result) {
			std::vector<int> candidate;
			for (size_t i = 0; i < m_inds.size(); ++i) {
				if (base_ != i) candidate.push_back(i);
			}
			if (result.size() != number)	result.resize(number);
			for (size_t i = 0; i < number; ++i) {
				size_t idx = global::ms_global->m_uniform[caller::Algorithm]->next_non_standard<size_t>(0, candidate.size() - i);
				result[i] = candidate[idx];
				if (idx != candidate.size() - (i + 1)) candidate[idx] = candidate[candidate.size() - (i + 1)];
			}
		}
		template<typename Individual>
		void population<Individual>::select_in_neighborhood(int number, std::vector<size_t> candidate, std::vector<size_t>& result) {
			if (result.size() != number)	result.resize(number);
			for (size_t i = 0; i < number; ++i) {
				int idx = global::ms_global->m_uniform[caller::Algorithm]->next_non_standard<int>(0, (int)candidate.size() - i);
				result[i] = candidate[idx];
				if (idx != candidate.size() - (i + 1)) candidate[idx] = candidate[candidate.size() - (i + 1)];
			}
		}
	}
}
#endif // !DE_POPULATION_H
