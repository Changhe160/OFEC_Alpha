/*************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Changhe Li
* Email: changhe.lw@gmail.com
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*************************************************************************/
// Created by Junchen Wang on Oct. 28, 2018.

#ifndef OFEC_EP_POP_H
#define OFEC_EP_POP_H

#include "../../../core/algorithm/population.h"
#include "../../../core/problem/continuous/continuous.h"
#include "individual.h"
#include <numeric>

namespace OFEC {
	namespace EP {
		template<typename Individual = individual>
		class population : public OFEC::population<Individual> {
		public:
			population() = default;
			population(size_t size_pop);
			population(const population &rhs);
			population(population&&rhs) noexcept;
			population& operator=(const population &rhs);
			population& operator=(population&&rhs) noexcept;
			evaluation_tag evolve() override;
			real& tau() { return m_tau; }
			real& tau_prime() { return m_tau_prime; }
			size_t& q() { return m_q; }
		protected:
			virtual void mutate();
			virtual void select();
		protected:
			std::vector<std::unique_ptr<Individual>> m_offspring;
			real m_tau = 0, m_tau_prime = 0;
			size_t m_q = 0;
		};

		template<typename Individual>
		population<Individual>::population(size_t size_pop) : OFEC::population<Individual>(size_pop, global::ms_global->m_problem->variable_size()), m_offspring(2 * size_pop) {
			size_t size_var = CONTINUOUS_CAST->variable_size();
			size_t num_obj = CONTINUOUS_CAST->objective_size();
			size_t num_con = CONTINUOUS_CAST->num_constraints();
			for (auto& i : m_offspring)
				i = std::move(std::unique_ptr<Individual>(new Individual(num_obj, num_con, size_var)));
		}

		template<typename Individual>
		population<Individual>::population(const population &rhs) : OFEC::population<Individual>(rhs) {
			for (size_t i = 0; i < rhs.m_offspring.size(); ++i)
				m_offspring.push_back(std::unique_ptr<Individual>(new Individual(*rhs.m_offspring[i])));
		}

		template<typename Individual>
		population<Individual>::population(population &&rhs) noexcept : OFEC::population<Individual>(std::move(rhs)), m_offspring(std::move(rhs.m_offspring)) {}

		template<typename Individual>
		population<Individual>& population<Individual>::operator=(const population &rhs) {
			if (this == &rhs) return *this;
			OFEC::population<Individual>::operator=(rhs);
			m_offspring.resize(rhs.m_offspring.size());
			for (size_t i = 0; i < rhs.m_offspring.size(); ++i)
				m_offspring[i] = std::move(std::unique_ptr<Individual>(new Individual(*rhs.m_offspring[i])));
			return *this;
		}

		template<typename Individual>
		population<Individual>& population<Individual>::operator=(population &&rhs) noexcept {
			if (this == &rhs) return *this;
			OFEC::population<Individual>::operator=(std::move(rhs));
			m_offspring = std::move(rhs.m_offspring);
			return *this;
		}

		template<typename Individual>
		void population<Individual>::mutate() {
			const size_t size_parents = this->m_inds.size();
			if (m_offspring.size() != 2 * size_parents) 
				throw myexcept("Error at EP::population::mutate(). The size of offspring must be 2 times of the size of parents.");
			for (size_t i = 0; i < size_parents; ++i)
				*m_offspring[i] = *this->m_inds[i];
			for (size_t i = size_parents; i < m_offspring.size(); i++) {
				real N = global::ms_global->m_normal[caller::Algorithm]->next();
				for (size_t j = 0; j < m_offspring[i]->variable().size(); ++j) {
					m_offspring[i]->variable()[j] = this->m_inds[i - size_parents]->variable()[j] + this->m_inds[i - size_parents]->eta()[j] * N;
					real N_j = global::ms_global->m_normal[caller::Algorithm]->next();
					m_offspring[i]->eta()[j] = this->m_inds[i - size_parents]->eta()[j] * exp(m_tau_prime*N + m_tau * N_j);
				}
			}
		}

		template<typename Individual>
		void population<Individual>::select() {
			std::vector<size_t> wins(m_offspring.size(), 0);
			std::vector<size_t> rand_seq(m_offspring.size());
			std::iota(rand_seq.begin(), rand_seq.end(), 0);
			for (size_t i = 0; i < m_offspring.size(); ++i) { 
				global::ms_global->m_uniform[caller::Algorithm]->shuffle(rand_seq.begin(), rand_seq.end());
				for (size_t idx = 0; idx < m_q; idx++)
					if (!m_offspring[rand_seq[idx]]->dominate(*m_offspring[i]))
						wins[i]++;			}
			std::vector<int> index;
			merge_sort(wins, wins.size(), index, false);
			for (size_t i = 0; i < this->m_inds.size(); ++i)
				*this->m_inds[i] = *m_offspring[index[i]];
		}

		template<typename Individual>
		evaluation_tag population<Individual>::evolve() {
			auto tag = evaluation_tag::Normal;

			mutate();

			for (size_t i = this->m_inds.size(); i < m_offspring.size(); ++i) {
				tag = m_offspring[i]->evaluate();
				if (tag != evaluation_tag::Normal) break;
			}

			if (tag == evaluation_tag::Normal) {
				select();
				++this->m_iter;
			}

			return tag;
		}
	}
}



#endif // !OFEC_EP_POP_H

