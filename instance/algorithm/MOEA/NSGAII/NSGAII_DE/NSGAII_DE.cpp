#include "NSGAII_DE.h"
#include "../../../../../core/problem/continuous/continuous.h"
#include <algorithm>

namespace OFEC {
	NSGAII_DE_pop::NSGAII_DE_pop(size_t size_pop) : MOEA_DE_pop(size_pop) {}

	void NSGAII_DE_pop::initialize() {
		MOEA_DE_pop::initialize();
		for (auto& i : this->m_inds)	m_offspring.emplace_back(*i);
		for (auto& i : this->m_inds)	m_offspring.emplace_back(*i);
	}

	evaluation_tag NSGAII_DE_pop::evolve() {
		if (this->m_inds.size() < 5) {
			throw myexcept("the population size cannot be smaller than 5@DE::population<Individual>::evolve()");
		}
		evaluation_tag tag = evaluation_tag::Normal;
		int m = 0;
		for (int i = 0; i < this->m_inds.size(); i++) {
			std::vector<size_t> p(3);
			p[0] = tournament_selection();
			while (1) { p[1] = tournament_selection();  	if (p[1] != p[0]) break; }
			while (1) { p[2] = tournament_selection();  	if (p[2] != p[0] && p[2] != p[1]) break; }
			cross_mutate(p, m_offspring[m]);
			tag = m_offspring[m].evaluate();
			if (tag != evaluation_tag::Normal) break;
			m++;
			m_offspring[m++] = *this->m_inds[i];
		}
		survivor_selection(this->m_inds, m_offspring);
		this->m_iter++;
		return tag;
	}
	
	NSGAII_DE::NSGAII_DE(param_map & v) :algorithm(v.at("algName")), m_pop(v.at("popSize")) {}

	void NSGAII_DE::initialize() {		
		m_pop.initialize();
		m_pop.evaluate();
	}

	void NSGAII_DE::run_() {
		while (!terminating()) {
			m_pop.evolve();
		}
	}

	void NSGAII_DE::record() {
		size_t evals = CONTINUOUS_CAST->evaluations();
		real IGD = CONTINUOUS_CAST->get_optima().IGD_to_PF(m_pop);
		measure::get_measure()->record(global::ms_global.get(), evals, IGD);
	}
}