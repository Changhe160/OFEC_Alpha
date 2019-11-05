#ifndef OFEC_GA_POPULATION_H
#define OFEC_GA_POPULATION_H

#include "../../../core/algorithm/population.h"
#include "../../../core/algorithm/individual.h"
#include <numeric>

namespace OFEC {
	namespace GA {
		template<typename Individual>
		class population : public OFEC::population<Individual> {
		protected:
			real m_cr = 0; // crossover probability
			real m_mr = 0; // mutation probability
			std::vector<size_t> m_rand_seq; // Random sequence of the population
		public:
			template<typename ... Args>
			population(size_t size_pop, Args&& ...args) : OFEC::population<Individual>(size_pop, std::forward<Args>(args)...),\
				m_rand_seq(size_pop) {
				std::iota(m_rand_seq.begin(), m_rand_seq.end(), 0);
			}
			void set_cr(real cr) { m_cr = cr; }
			void set_mr(real mr) { m_mr = mr; }
		protected:
			virtual void crossover(size_t idx_parent1, size_t idx_parent2, Individual& child1, Individual& child2) = 0;
			virtual void mutate(Individual &ind) = 0;
			size_t tournament_selection(size_t tournament_size = 2);
		};

		template<typename Individual>
		size_t population<Individual>::tournament_selection(size_t tournament_size) {
			global::ms_global->m_uniform[caller::Algorithm]->shuffle(m_rand_seq.begin(), m_rand_seq.end());
			size_t idx_best = m_rand_seq[0];
			for (size_t k = 1; k < tournament_size; ++k)
				if (this->m_inds[m_rand_seq[k]]->dominate(*this->m_inds[idx_best]))
					idx_best = m_rand_seq[k];
			return idx_best;
		}
	}
}

#endif // !OFEC_GA_INDIVIDUAL_H

