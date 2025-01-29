#include "./canonical_de.h"

//#ifdef OFEC_PLAYBACK
//#include <buffer/datum_inclusion.h>
//#endif // OFEC_PLAYBACK
//
//#ifdef OFEC_STATISTICS
//#include <record/datum_inclusion.h>
//#endif // OFEC_STATISTICS

#include "../../../../../../datum/datum_inclusion.h"


namespace ofec {
	void CanonicalDE::addInputParameters() {
		m_input_parameters.add("population size", new RangedSizeT(m_population_size, 5, 1000, 20));
		m_input_parameters.add("scaling factor", new RangedReal(m_scaling_factor, 0, 2, 0.5));
		m_input_parameters.add("crossover rate", new RangedReal(m_crossover_rate, 0, 1, 0.6));
		m_input_parameters.add(
			"mutation strategy", 
			new Enumeration(
				m_mutation_strategy,
				{ 
					"rand/1", 
					"best/1", 
					"current-to-best/1",
					"best/2",
					"rand/2", 
					"rand-to-best/1",
					"current-to-rand/1" 
				},
				de::MutateStrategy::kRand1
			)
		);
		m_input_parameters.add(
			"recombine strategy", 
			new Enumeration(
				m_recombine_strategy,
				{ 
					"binomial", 
					"exponential" 
				}, 
				de::RecombineStrategy::kBinomial
			)
		);
	}

	void CanonicalDE::initialize_(Environment *env) {
		Algorithm::initialize_(env);

	}

	void CanonicalDE::run_(Environment *env) {
		initializePopulation(m_population_size, env);
		while (!terminating()) {
			m_population->evolve(env, m_random.get());
#ifdef OFEC_DATUM_MULTI_POP_H
			datumUpdated(env, g_multi_pop);
#endif // OFEC_DATUM_MULTI_POP_H
		}
	}

	void CanonicalDE::initializePopulation(size_t pop_size, Environment *env) {
		m_population.reset(new PopulationDE<>(pop_size, env));
		m_population->crossoverRate() = m_crossover_rate;
		m_population->scalingFactor() = m_scaling_factor;
		m_population->mutationStrategy() = m_mutation_strategy;
		m_population->recombineStrategy() = m_recombine_strategy;
		m_population->initialize(env, m_random.get());
		m_population->evaluate(env);
#ifdef OFEC_DATUM_MULTI_POP_H
		g_multi_pop.pops.clear();
		g_multi_pop.pops.resize(1);
		for (size_t i = 0; i < m_population->size(); ++i) {
			g_multi_pop.pops[0].push_back(&m_population->at(i));
		}
		datumUpdated(env, g_multi_pop);
#endif // OFEC_DATUM_MULTI_POP_H
	}
}
