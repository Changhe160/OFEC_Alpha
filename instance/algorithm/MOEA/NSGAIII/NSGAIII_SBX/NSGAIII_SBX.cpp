#include "NSGAIII_SBX.h"
#include "../../../../../core/problem/continuous/continuous.h"

namespace OFEC {
	NSGAIII_SBX_pop::NSGAIII_SBX_pop(size_t size_pop) : SBX_pop<>(size_pop), NSGAIII<individual<>>(size_pop) {}

	void NSGAIII_SBX_pop::initialize() {
		SBX_pop::initialize();
		for (auto& i : m_inds)	m_offspring.emplace_back(*i);
		for (auto& i : m_inds)	m_offspring.emplace_back(*i);
	}

	evaluation_tag NSGAIII_SBX_pop::evolve() {
		if (m_inds.size() % 2 != 0)
			throw myexcept("population size should be even @NSGAIII_SBXRealMu::evolve_mo()");
		for (size_t i = 0; i < m_inds.size(); i += 2) {
			std::vector<size_t> p(2);
			p[0] = tournament_selection();
			do { p[1] = tournament_selection(); } while (p[1] == p[0]);
			crossover(p[0], p[1], m_offspring[i], m_offspring[i + 1]);
			mutate(m_offspring[i]);
			mutate(m_offspring[i + 1]);
		}
		for (auto& i : m_offspring)  i.evaluate();
		for (size_t i = 0; i < m_inds.size(); ++i)  m_offspring[i + m_inds.size()] = *m_inds[i];
		survivor_selection(m_inds, m_offspring);
		m_iter++;
		return evaluation_tag::Normal;
	}

	NSGAIII_SBX::NSGAIII_SBX(param_map & v) : algorithm(v.at("algName")), m_pop(v.at("popSize")) {}

	void NSGAIII_SBX::initialize() {
		m_pop.initialize();
		m_pop.evaluate();
	}

	void NSGAIII_SBX::run_() {
		while (!terminating()) {
			m_pop.evolve();
		}
	}

	void NSGAIII_SBX::record() {
		size_t evals = CONTINUOUS_CAST->evaluations();
		real IGD = CONTINUOUS_CAST->get_optima().IGD_to_PF(m_pop);
		measure::get_measure()->record(global::ms_global.get(), evals, IGD);
	}


}