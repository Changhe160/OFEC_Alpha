#ifndef NSGAII_H
#define NSGAII_H

#include "../../../../core/algorithm/algorithm.h"
#include "../../../../core/algorithm/individual.h"
#include "../../../../utility/functional.h"
#include "NSGAIIpopulation.h"
#include <list>
#include "../../../../core/problem/continuous/continuous.h"
#include "../../../../core/measure/measure.h"

namespace OFEC {
	template<typename Individual = individual<>>
	class NSGAII : public algorithm
	{
	public:
		NSGAII(param_map &v);
		~NSGAII() {}
		evaluation_tag run_();
		virtual void sort(); 
	protected:
		void eval_dens(); 
		virtual void evolve_mo();
		int tour_selection();

    protected:
		double m_GAratio = global::ms_arg.find(param_GAratio) == global::ms_arg.end() ? 1.0 : (int)(global::ms_arg[param_GAratio]);
		NSGAIIpopulation<Individual> m_parent, m_offspring;
		int m_objcomp; // Cumulative number of objective comparisons
		int m_numrank;
	};
	using FNS_NSGAII = NSGAII<>;

	template<typename Individual>
	NSGAII<Individual>::NSGAII(param_map & v) : m_parent(v[param_popSize]), m_offspring(size_t(m_parent.size() * (1.0 + m_GAratio))), algorithm(v[param_algName]), m_objcomp(0), m_numrank(0) {
		for (size_t i = 0; i < m_parent.size(); i++) {
			m_parent[i].evaluate();
		}
		set_termination(new term_max_evals(v));
	}
	template<typename Individual>
	evaluation_tag NSGAII<Individual>::run_() {

		int evals = global::ms_global->m_problem->total_evaluations();
		double IGD = CONTINOUS_CAST->get_optima().distance_to_optimal_obj(m_parent);

		std::cout << evals << ", " << IGD << std::endl;

		measure::ms_measure->record(global::ms_global.get(), evals, IGD, m_objcomp);

		// evolution
		while (!terminating())
		{
			evolve_mo();
			sort();
			eval_dens();

			evals = global::ms_global->m_problem->total_evaluations();
			if (evals % (int)global::ms_arg[param_sampleFre] == 0) {
				IGD = CONTINOUS_CAST->get_optima().distance_to_optimal_obj(m_parent);
				std::cout << evals << ", " << IGD << std::endl;
				measure::ms_measure->record(global::ms_global.get(), evals, IGD, m_objcomp);
			}
		}
		return evaluation_tag::Normal;
	}

	template<typename Individual>
	void NSGAII<Individual>::sort() {
		int pop_size = m_offspring.size();
		std::vector<int> rank_(pop_size, 0);
		std::vector<int> count(pop_size, 0);
		std::list<std::vector<int> > cset(pop_size, std::vector<int>(pop_size));

		for (size_t i = 0; i < pop_size; i++)
		{
			m_offspring[i].set_rank(-1);
		}

		auto i = cset.begin();
		for (size_t k = 0; k < pop_size; k++, ++i) {
			for (size_t j = 0; j<pop_size; j++) {
				if (k != j) {
					std::pair<dominationship, int> result = objective_compare(m_offspring[j].get_objective(), m_offspring[k].get_objective(), global::ms_global->m_problem->opt_mode());
					m_objcomp += result.second;
					if (result.first == dominationship::Dominating)
						rank_[k]++;
					else if (result.first == dominationship::Dominated) {
						(*i)[count[k]] = j;
						count[k]++;
					}
				}
			}
		}

		int m_curRank = 0;
		std::vector<int> rank2(pop_size);
		while (1)
		{
			int stop_count = 0;
			for (size_t k = 0; k<pop_size; k++)
				rank2[k] = rank_[k];
			auto i = cset.begin();
			for (size_t k = 0; k<pop_size; k++, ++i)
			{
				if (m_offspring[k].rank() == -1 && rank_[k] == 0)
				{
					m_offspring[k].set_rank(m_curRank);
					for (size_t j = 0; j<count[k]; j++)
					{
						//int id =	cset[k][j];
						int id = (*i)[j];
						rank2[id]--;
						stop_count++;
					}
				}
			}

			for (size_t k = 0; k<pop_size; k++)
				rank_[k] = rank2[k];

			m_curRank++;
			if (stop_count == 0)
				break;
		}
	}

	template<typename Individual>
	void NSGAII<Individual>::eval_dens() {
		int numobj = global::ms_global->m_problem->objective_size();
		int pops = 0;  //indicate parent population size be 0
		int size = m_offspring.size();
		int rank = 0;
		while (1) {
			int count = 0;
			for (size_t i = 0; i<size; i++)
				if (m_offspring[i].rank() == rank)
					count++;

			int size2 = pops + count;
			if (size2 > m_parent.size()) {
				break;
			}

			for (size_t i = 0; i<size; i++)
				if (m_offspring[i].rank() == rank)
				{
					m_parent[pops] = m_offspring[i];
					++pops;
				}

			rank++;
			if (pops >= m_parent.size()) break;
		}

		if (pops<m_parent.size()) {
			std::vector<int> list;
			// save the individuals in the overflowed front
			for (size_t i = 0; i<size; i++)
				if (m_offspring[i].rank() == rank)
					list.push_back(i);
			int s2 = list.size();
			std::vector<double> density(s2);
			std::vector<double> obj(s2);
			std::vector<int> idx(s2);
			std::vector<int> idd(s2);

			for (size_t i = 0; i<s2; i++) {
				idx[i] = i;
				density[i] = 0;
			}

			for (size_t j = 0; j<numobj; j++) {
				for (size_t i = 0; i<s2; i++) {
					idd[i] = i;
					obj[i] = m_offspring[list[i]].get_objective()[j];
				}
				//gMinfastsort(obj,idd,s2,s2);
				quick_sort(obj, s2, idd, true, 0, s2 - 1, s2);
				density[idd[0]] += -1.0e+30;
				density[idd[s2 - 1]] += -1.0e+30;
				for (int k = 1; k<s2 - 1; k++)
					density[idd[k]] += -(obj[idd[k]] - obj[idd[k - 1]] + obj[idd[k + 1]] - obj[idd[k]]);
			}
			idd.clear();
			obj.clear();

			int s3 = m_parent.size() - pops;

			//gMinfastsort(density,idx,s2,s3);
			quick_sort(density, s2, idx, true, 0, s2 - 1, s3);
			for (size_t i = 0; i<s3; i++)
			{
				m_parent[pops] = m_offspring[list[idx[i]]];
				++pops;
			}

			density.clear();
			idx.clear();
			list.clear();
		}
		m_numrank = rank;
	}

	template<typename Individual>
	void NSGAII<Individual>::evolve_mo() {
		int m = m_parent.size();
		if (m_parent.size() % 2 != 0)
			throw myexcept("population size should be even @NSGAII::evolve_mo()");
		for (int n = 0; n<m_parent.size(); n += 2)
		{
			m_offspring[n] = m_parent[n];
			m_offspring[n + 1] = m_parent[n + 1];
			if ((m + 1) < m_offspring.size()) {
				std::vector<int> p(2);
				p[0] = tour_selection();
				while (1) { p[1] = tour_selection();  	if (p[1] != p[0]) break; }
				m_parent.cross_mutate(p, &m_offspring[m], &m_offspring[m + 1]);
			}
			else if ((m + 1) > m_offspring.size()) {

			}
			else {
				std::vector<int> p(2);
				p[0] = tour_selection();
				while (1) { p[1] = tour_selection();  	if (p[1] != p[0]) break; }
				m_parent.cross_mutate(p, &m_offspring[m], &m_offspring[m]);
			}

			m += 2;

		}
	}

	template<typename Individual>
	int NSGAII<Individual>::tour_selection() {
		int p1 = global::ms_global->m_uniform[caller::Algorithm]->next_non_standard(0, m_parent.size());
		int p2 = global::ms_global->m_uniform[caller::Algorithm]->next_non_standard(0, m_parent.size());

		if (m_parent[p1].rank() < m_parent[p2].rank())
			return p1;
		else
			return p2;
	}
}

#endif NSGAII_H